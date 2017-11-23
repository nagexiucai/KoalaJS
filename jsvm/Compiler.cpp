#include "Compiler.h"
#include "utils/File/File.h"
#include "utils/String/StringUtil.h"
#include <assert.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace std;

#ifdef __GNUC__
#define vsprintf_s vsnprintf
#define sprintf_s snprintf
#define _strdup strdup
#endif

// ----------------------------------------------------------------------------------- CSCRIPT

Compiler::Compiler() {
	l = 0;
	cindex = 0;
	cwd = "";
	cname = "";
}

Compiler::~Compiler() {
}

//added by Misa.Z for running file
bool Compiler::run(const std::string &fname, Bytecode* bc, bool debug, bool end) {
	bool res = false;
	std::string oldCwd = cwd;
	std::string oldCName = cname;

	cname = File::getFullname(cwd, fname);
	cwd = File::getPath(cname);

	//check if included.
	size_t sz = included.size();
	for(size_t i=0; i<sz; ++i) {
		string fn = included[i];
		if(fn == cname)
			return true; //included.
	}


	std::string input = File::read(cname);
	if(input.length() > 0) {
		bytecode = bc;
		if(debug)
			bytecode->setCompiler(this);

		res = exec(input, bc, end);

		if(debug)
			bc->getDebug()->loadFiles();
	}
	else {
		ERR("Can not run file \"%s\"!\n", cname.c_str());
	}
	included.push_back(cname);

	if(oldCwd.length() > 0)
		cwd = oldCwd;
	if(oldCName.length() > 0)
		cname = oldCName;
	return res;
}

bool Compiler::exec(const std::string &code, Bytecode* bc, bool end) {
	bytecode = bc;
	CScriptLex* old = l;
	l = new CScriptLex(code);

	try {
		while (l->tk) {
			statement();
		}
		if(end)
			bytecode->gen(INSTR_END);
	} catch (CScriptException *e) {
		std::string msg;
		std::stringstream ss;
		ss << "Error at " << cname << ": " << l->getPosition().c_str() << ": " << e->text.c_str();
		ERR("%s\n", ss.str().c_str());

		delete l;
		delete e;
		return false;
	}
	delete l;
	l = old;
	return true;
}

LEX_TYPES Compiler::statement(bool pop) {
	LEX_TYPES ret = LEX_EOF;

	if (l->tk==LEX_R_INCLUDE) {
		l->chkread(LEX_R_INCLUDE);

		bytecode->gen(INSTR_BLOCK); 
		run(l->tkStr, bytecode, bytecode->isDebug(), false);
		bytecode->gen(INSTR_BLOCK_END); 
		pop = false;

		l->chkread(LEX_STR);
		l->chkread(';');
	}
	else if (l->tk==LEX_ID    ||
			l->tk==LEX_INT   ||
			l->tk==LEX_FLOAT ||
			l->tk==LEX_STR   ||
			l->tk==LEX_PLUSPLUS   ||
			l->tk==LEX_MINUSMINUS ||
			l->tk=='-'    ) {
		/* Execute a simple statement that only contains basic arithmetic... */
		base();
		l->chkread(';');
	}
	else if (l->tk=='{') {
		/* A block of code */
		block();
	}
	else if (l->tk==';') {
		/* Empty statement - to allow things like ;;; */
		l->chkread(';');
	}
	else if (l->tk==LEX_R_BREAK){
		l->chkread(LEX_R_BREAK);
		l->chkread(';');
		//reserve for jump to end of loop.
		PC pc = bytecode->reserve();
		Loop* l = getLoop();
		if(l != NULL)
			l->breaks.push_back(pc);
	} 
	else if (l->tk==LEX_R_CONTINUE){
		l->chkread(LEX_R_CONTINUE);
		l->chkread(';');
		//reserve for jump to begin of loop condition.
		PC pc = bytecode->reserve();
		Loop* l = getLoop();
		if(l != NULL)
			l->continues.push_back(pc);
	}
	else if (l->tk==LEX_R_VAR || l->tk == LEX_R_CONST) {
		pop = false;
		bool beConst;
		if(l->tk == LEX_R_VAR) {
			l->chkread(LEX_R_VAR);
			beConst = false;
		}
		else {
			l->chkread(LEX_R_CONST);
			beConst = true;
		}

		while (l->tk != ';') {
			string vname = l->tkStr;
			l->chkread(LEX_ID);
			// now do stuff defined with dots
			/*while (l->tk == '.') {
				l->chkread('.');
				vname = vname + "." + l->tkStr;
				l->chkread(LEX_ID);
			}*/
			bytecode->gen(beConst ? INSTR_CONST : INSTR_VAR, vname);
			// sort out initialiser
			if (l->tk == '=') {
				l->chkread('=');
				bytecode->gen(INSTR_LOAD, vname);
				base();
				bytecode->gen(INSTR_ASIGN);
				bytecode->gen(INSTR_POP);
			}

			if (l->tk != ';')
				l->chkread(',');
		}      
		l->chkread(';');
	}
	else if(l->tk==LEX_R_FUNCTION) {
		l->chkread(LEX_R_FUNCTION);
		string name;
		defFunc(name);
		bytecode->gen(INSTR_MEMBERN, name);
		pop = false;
	}
	else if(l->tk==LEX_R_CLASS) {
		defClass();
		pop = false;
	}
	else if (l->tk==LEX_R_RETURN) {
		l->chkread(LEX_R_RETURN);
		pop = false;
		if (l->tk != ';') {
			base();
			bytecode->gen(INSTR_RETURNV);
		}
		else {
			bytecode->gen(INSTR_RETURN);
		}
		l->chkread(';');
	} 
	else if (l->tk==LEX_R_WHILE) {
		Loop loop;
		loopStack.push(loop);

		l->chkread(LEX_R_WHILE);
		l->chkread('(');
		PC cpc = bytecode->getPC();
		base(); //condition
		l->chkread(')');
		PC pc = bytecode->reserve();
		statement();
		bytecode->addInstr(cpc, INSTR_JMPB); //coninue anchor;
		bytecode->setInstr(pc, INSTR_NJMP); // end anchor;

		setLoopBreaks(getLoop(), bytecode->getPC());
		setLoopContinues(getLoop(), cpc);
		loopStack.pop();
		pop = false;
	}
	else if (l->tk==LEX_R_IF) {
		l->chkread(LEX_R_IF);
		l->chkread('(');
		base(); //condition
		l->chkread(')');
		PC pc = bytecode->reserve();
		statement();

		if (l->tk==LEX_R_ELSE) {
			l->chkread(LEX_R_ELSE);
			PC pc2 = bytecode->reserve();
			bytecode->setInstr(pc, INSTR_NJMP);
			statement();
			bytecode->setInstr(pc2, INSTR_JMP);
		}
		else {
			bytecode->setInstr(pc, INSTR_NJMP);
		}
		pop = false;
	}
	else if (l->tk==LEX_R_FOR) {
		Loop loop;
		loopStack.push(loop);

		l->chkread(LEX_R_FOR);
		l->chkread('(');
		statement(); //init
		//l->chkread(';');
		PC cpc = bytecode->getPC();
		base(); //continue
		PC breakPC = bytecode->reserve();
		PC loopPC = bytecode->reserve();
		l->chkread(';');
		PC ipc = bytecode->getPC();
		base(); //iterator
		bytecode->gen(INSTR_POP);
		l->chkread(')');
		bytecode->addInstr(cpc, INSTR_JMPB); //continue anchor
		bytecode->setInstr(loopPC, INSTR_JMP);
		statement();
		bytecode->addInstr(ipc, INSTR_JMPB); //iterator anchor
		bytecode->setInstr(breakPC, INSTR_NJMP); //end anchor
		
		setLoopBreaks(getLoop(), bytecode->getPC());
		setLoopContinues(getLoop(), cpc);
		loopStack.pop();
		pop = false;
	}
	else {
		l->chkread(LEX_EOF);
	}

	if(pop)
		bytecode->gen(INSTR_POP);
	return ret;
}

LEX_TYPES Compiler::base() {
	LEX_TYPES ret = LEX_EOF;
	ret = ternary();
	if (l->tk=='=' || 
			l->tk==LEX_PLUSEQUAL ||
			l->tk==LEX_MULTIEQUAL ||
			l->tk==LEX_DIVEQUAL ||
			l->tk==LEX_MODEQUAL ||
			l->tk==LEX_MINUSEQUAL) {
		LEX_TYPES op = l->tk;
		l->chkread(l->tk);
		base();
		// sort out initialiser
		if (op == '=') 
			bytecode->gen(INSTR_ASIGN);
		else if(op == LEX_PLUSEQUAL)
			bytecode->gen(INSTR_PLUSEQ);
		else if(op == LEX_MINUSEQUAL)
			bytecode->gen(INSTR_MINUSEQ);
		else if(op == LEX_MULTIEQUAL)
			bytecode->gen(INSTR_MULTIEQ);
		else if(op == LEX_DIVEQUAL)
			bytecode->gen(INSTR_DIVEQ);
		else if(op == LEX_MODEQUAL)
			bytecode->gen(INSTR_MODEQ);
	}
	return ret;
}

LEX_TYPES Compiler::unary() {
	LEX_TYPES ret = LEX_EOF;
	bool no = false;
	if (l->tk == '!') {
		l->chkread('!');
		no = true;
	}

	ret = factor();

	if(no)
		bytecode->gen(INSTR_NOT);
	return ret;	
}

LEX_TYPES Compiler::term() {
	LEX_TYPES ret = LEX_EOF;
	ret = unary();

	while (l->tk=='*' || l->tk=='/' || l->tk=='%') {
		LEX_TYPES op = l->tk;
		l->chkread(l->tk);
		unary();

		if(op == '*')
			bytecode->gen(INSTR_MULTI);
		else if(op == '/')
			bytecode->gen(INSTR_DIV);
		else
			bytecode->gen(INSTR_MOD);
	}

	return ret;	
}

LEX_TYPES Compiler::expr() {
	LEX_TYPES ret = LEX_EOF;
	LEX_TYPES pre = l->tk;

	if (l->tk=='-') {
		l->chkread('-');
	}
	else if(l->tk==LEX_PLUSPLUS) {
		l->chkread(LEX_PLUSPLUS);
	}
	else if(l->tk==LEX_MINUSMINUS) {
		l->chkread(LEX_MINUSMINUS);
	}

	ret = term();
	if (pre == '-') {
		bytecode->gen(INSTR_NEG);
	}
	else if(pre==LEX_PLUSPLUS) {
		bytecode->gen(INSTR_PPLUS_PRE);
	}
	else if(pre==LEX_MINUSMINUS) {
		bytecode->gen(INSTR_MMINUS_PRE);
	}

	while (l->tk=='+' || l->tk=='-' ||
			l->tk==LEX_PLUSPLUS || l->tk==LEX_MINUSMINUS) {
		int op = l->tk;
		l->chkread(l->tk);
		if (op==LEX_PLUSPLUS) {
			bytecode->gen(INSTR_PPLUS);
		}
		else if(op==LEX_MINUSMINUS) {
			bytecode->gen(INSTR_MMINUS);
		}
		else {
			ret = term();
			if(op== '+') {
				bytecode->gen(INSTR_PLUS);
			}
			else if(op=='-') {
				bytecode->gen(INSTR_MINUS);
			}
		}
	}

	return ret;	
}

LEX_TYPES Compiler::shift() {
	LEX_TYPES ret = LEX_EOF;
	ret = expr();

	if (l->tk==LEX_LSHIFT || l->tk==LEX_RSHIFT || l->tk==LEX_RSHIFTUNSIGNED) {
		int op = l->tk;
		l->chkread(op);
		ret = base();

		if (op==LEX_LSHIFT) 
			bytecode->gen(INSTR_LSHIFT);
		else if (op==LEX_RSHIFT)
			bytecode->gen(INSTR_RSHIFT);
		else
			bytecode->gen(INSTR_URSHIFT);
	}
	return ret;	
}

LEX_TYPES Compiler::condition() {
	LEX_TYPES ret = LEX_EOF;
	ret = shift();

	while (l->tk==LEX_EQUAL || l->tk==LEX_NEQUAL ||
			l->tk==LEX_LEQUAL || l->tk==LEX_GEQUAL ||
			l->tk=='<' || l->tk=='>') {
		int op = l->tk;
		l->chkread(l->tk);
		ret = shift();

		if(op == LEX_EQUAL)
			bytecode->gen(INSTR_EQ);
		else if(op == LEX_NEQUAL)
			bytecode->gen(INSTR_NEQ);
		else if(op == LEX_LEQUAL)
			bytecode->gen(INSTR_LEQ);
		else if(op == LEX_GEQUAL)
			bytecode->gen(INSTR_GEQ);
		else if(op == '>')
			bytecode->gen(INSTR_GRT);
		else if(op == '<')
			bytecode->gen(INSTR_LES);
	}

	return ret;	
}

LEX_TYPES Compiler::logic() {
	LEX_TYPES ret = LEX_EOF;
	ret = condition();

	while (l->tk=='&' || l->tk=='|' || l->tk=='^' || l->tk==LEX_ANDAND || l->tk==LEX_OROR) {
		int op = l->tk;
		l->chkread(l->tk);
		ret = condition();

		if (op==LEX_ANDAND) {
			bytecode->gen(INSTR_AAND);
		} 
		else if (op==LEX_OROR) {
			bytecode->gen(INSTR_OOR);
		}
		else if (op=='|') {
			bytecode->gen(INSTR_OR);
		}
		else if (op=='&') {
			bytecode->gen(INSTR_AND);
		}
		else if (op=='^') {
			bytecode->gen(INSTR_XOR);
		}
	}
	return ret;	
}

LEX_TYPES Compiler::ternary() {
	LEX_TYPES ret = LEX_EOF;
	ret = logic();
	
	if (l->tk=='?') {
		PC pc1 = bytecode->reserve(); //keep for jump
		l->chkread('?');
		base(); //first choice
		PC pc2 = bytecode->reserve(); //keep for jump
		l->chkread(':');
		bytecode->setInstr(pc1, INSTR_NJMP);
		base(); //second choice
		bytecode->setInstr(pc2, INSTR_JMP);
	} 
	return ret;	
}

LEX_TYPES Compiler::callFunc(int &argNum) {
	LEX_TYPES ret = LEX_EOF;
	l->chkread('(');
	
	argNum = 0;
	while(true) {
		PC pc1 = bytecode->getPC();
		base();
		PC pc2 = bytecode->getPC();
		if(pc2 > pc1) //not empty, means valid arguemnt.
			argNum++;

		if (l->tk!=')')
			l->chkread(',');	
		else
			break;
	}
	l->chkread(')');
	return ret;
}

LEX_TYPES Compiler::defFunc(string& name) {
	LEX_TYPES ret = LEX_EOF;
	// actually parse a function...
	name = "";
	/* we can have functions without names */
	if (l->tk == LEX_ID) {
		name = l->tkStr;
		l->chkread(LEX_ID);
	}
	
	if(l->tk == LEX_ID) { //class get/set token
		if(name == "get") {
			name = l->tkStr;
			l->chkread(LEX_ID);
			bytecode->gen(INSTR_FUNC_GET);
		}
		else if(name == "set") {
			name = l->tkStr;
			l->chkread(LEX_ID);
			bytecode->gen(INSTR_FUNC_SET);
		}
	}
	else {
		bytecode->gen(INSTR_FUNC);
	}
	//do arguments
	l->chkread('(');
	while (l->tk!=')') {
		bytecode->gen(INSTR_VAR, l->tkStr);
		l->chkread(LEX_ID);
		if (l->tk!=')') l->chkread(',');
	}
	l->chkread(')');
	PC pc = bytecode->reserve();
	block();
	
	OprCode op = bytecode->getInstr(bytecode->getPC() - 1) >> 16;

	if(op != INSTR_RETURN && op != INSTR_RETURNV)
		bytecode->gen(INSTR_RETURN);
	bytecode->setInstr(pc, INSTR_JMP);
	return ret;
}

LEX_TYPES Compiler::defClass() {
	LEX_TYPES ret = LEX_EOF;
	// actually parse a function...
	l->chkread(LEX_R_CLASS);
	std::string name = "";
	/* we can have functions without names */
	if (l->tk==LEX_ID) {
		name = l->tkStr;
		l->chkread(LEX_ID);
	}

	bytecode->gen(INSTR_CLASS, name);
	//do arguments
	l->chkread('{');
	while (l->tk!='}') {
		defFunc(name);
		bytecode->gen(INSTR_MEMBERN, name);
	}
	l->chkread('}');
	bytecode->gen(INSTR_CLASS_END);

	return ret;
}


LEX_TYPES Compiler::factor() {
	LEX_TYPES ret = LEX_EOF;

	if (l->tk=='(') {
		l->chkread('(');
		base();
		l->chkread(')');
	}
	else if (l->tk==LEX_R_TRUE) {
		l->chkread(LEX_R_TRUE);
		bytecode->gen(INSTR_TRUE);
	}
	else if (l->tk==LEX_R_FALSE) {
		l->chkread(LEX_R_FALSE);
		bytecode->gen(INSTR_FALSE);
	}
	else if (l->tk==LEX_R_NULL) {
		l->chkread(LEX_R_NULL);
		bytecode->gen(INSTR_NULL);
	}
	else if (l->tk==LEX_R_UNDEFINED) {
		l->chkread(LEX_R_UNDEFINED);
		bytecode->gen(INSTR_UNDEF);
	}
	else if (l->tk==LEX_INT) {
		bytecode->gen(INSTR_INT, l->tkStr);
		l->chkread(LEX_INT);
	}
	else if (l->tk==LEX_FLOAT) {
		bytecode->gen(INSTR_FLOAT, l->tkStr);
		l->chkread(LEX_FLOAT);
	}
	else if (l->tk==LEX_STR) {
		bytecode->gen(INSTR_STR, l->tkStr);
		l->chkread(LEX_STR);
	}
	else if(l->tk==LEX_R_FUNCTION) {
		l->chkread(LEX_R_FUNCTION);
		string name;
		defFunc(name);
	}
	else if(l->tk==LEX_R_CLASS) {
		defClass();
	}
	else if (l->tk==LEX_R_NEW) {
		// new -> create a new object
		l->chkread(LEX_R_NEW);
		std::string className = l->tkStr;
		l->chkread(LEX_ID);
		if (l->tk == '(') {
			//l->chkread('(');
			int argNum;
			callFunc(argNum);
			//l->chkread(')');
			if(argNum > 0)
				className = className + "$" + StringUtil::from(argNum);
			bytecode->gen(INSTR_NEW, className);
		}
	}
	if (l->tk=='{') {
		/* JSON-style object definition */
		l->chkread('{');
		bytecode->gen(INSTR_OBJ);
		while (l->tk != '}') {
			string id = l->tkStr;
			// we only allow strings or IDs on the left hand side of an initialisation
			if (l->tk==LEX_STR) l->chkread(LEX_STR);
			else l->chkread(LEX_ID);
			l->chkread(':');
			base();
			bytecode->gen(INSTR_MEMBERN, id);
			// no need to clean here, as it will definitely be used
			if (l->tk != '}') l->chkread(',');
		}
		bytecode->gen(INSTR_OBJ_END);
		l->chkread('}');
	}
	else if(l->tk==LEX_ID) {
		string name = l->tkStr;
		l->chkread(LEX_ID);

		bool load = true;
		vector<string> names;
		while (l->tk=='(' || l->tk=='.' || l->tk=='[') {
			if (l->tk=='(') { // ------------------------------------- Function Call
				int argNum;
				callFunc(argNum);
				StringUtil::split(name, '.', names);
				name = "";
				int sz = (int)(names.size()-1);
				string s = names[sz];
				if(argNum > 0)
					s = s + "$" + StringUtil::from(argNum);
					
				if(sz == 0 && load) {
					bytecode->gen(INSTR_LOAD, "this");	
					bytecode->gen(INSTR_CALL, s);	
				}
				else {
					for(int i=0; i<sz; i++) {
						bytecode->gen(load ? INSTR_LOAD:INSTR_GET, names[i]);	
						load = false;
					}
					bytecode->gen(INSTR_CALLO, s);	
				}
				load = false;
			} 
			else if (l->tk == '.') { // ------------------------------------- Record Access
				l->chkread('.');
				if(name.length() == 0)
					name = l->tkStr;
				else 
					name = name + "." + l->tkStr;
				l->chkread(LEX_ID);
			} 
			else { // ------------------------------------- Array Access
				StringUtil::split(name, '.', names);
				name = "";
				int sz = (int)names.size();
				for(int i=0; i<sz; i++) {
					bytecode->gen(load ? INSTR_LOAD:INSTR_GET, names[i]);	
					load = false;
				}

				l->chkread('[');
				base();
				l->chkread(']');
				bytecode->gen(INSTR_ARRAY_AT);
			} 
		}
		if(name.length() > 0) {
			StringUtil::split(name, '.', names);
			name = "";
			int sz = (int)names.size();
			for(int i=0; i<sz; i++) {
				bytecode->gen(load ? INSTR_LOAD:INSTR_GET, names[i]);	
				load = false;
			}
		}
	}
	else if (l->tk=='[') {
		/* JSON-style array */
		l->chkread('[');
		bytecode->gen(INSTR_ARRAY);
		while (l->tk != ']') {
			base();
			bytecode->gen(INSTR_MEMBER);
			if (l->tk != ']') l->chkread(',');
		}
		l->chkread(']');
		bytecode->gen(INSTR_ARRAY_END);
	}

	return ret;
}

LEX_TYPES Compiler::block() {
	LEX_TYPES ret = LEX_EOF;
	l->chkread('{');
	while (l->tk && l->tk!='}'){
		ret = statement();
	}
	l->chkread('}');
	return ret;
}

Loop* Compiler::getLoop() {
	if(loopStack.empty())
		return NULL;
	return &loopStack.top();
}

void Compiler::setLoopBreaks(Loop* loop, PC pc) {
	if(loop != NULL) {
		for(size_t i=0; i<loop->breaks.size(); ++i) {
			bytecode->setInstr(loop->breaks[i], INSTR_JMP, pc);
		}
	}
}

void Compiler::setLoopContinues(Loop* loop, PC pc) {
	if(loop != NULL) {
		for(size_t i=0; i<loop->continues.size(); ++i) {
			bytecode->setInstr(loop->continues[i], INSTR_JMPB, pc);
		}
	}
}
