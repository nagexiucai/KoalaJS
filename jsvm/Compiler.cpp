#include "Compiler.h"
#include "../libs/File/File.h"
#include "../libs/String/StringUtil.h"
#include <assert.h>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static unsigned char cmap[256]={
	//+0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,//00
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//10
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//20
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,//30
	0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,//40
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 4,//50
	0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,//60
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0,//70
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//80
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//90
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//A0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//B0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//C0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//D0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//E0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//F0
}; 
using namespace std;

#ifdef __GNUC__
#define vsprintf_s vsnprintf
#define sprintf_s snprintf
#define _strdup strdup
#endif


// ----------------------------------------------------------------------------------- Utils
bool isWhitespace(unsigned char ch) {
	return (cmap[ch]&1);//(ch==' ') || (ch=='\t') || (ch=='\n') || (ch=='\r');
}
////////////////////////////////////////////////////////////////////////////////
bool isNumeric(unsigned char ch) {
	return (cmap[ch]&2);//(ch>='0') && (ch<='9');
}
////////////////////////////////////////////////////////////////////////////////
bool isNumber(const std::string &str) {
	for (size_t i=0;i<str.size();i++){
		if (!isNumeric(str[i])) return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
bool isHexadecimal(unsigned char ch) {
	return ((ch>='0') && (ch<='9')) ||
		((ch>='a') && (ch<='f')) ||
		((ch>='A') && (ch<='F'));
}
////////////////////////////////////////////////////////////////////////////////
bool isAlpha(unsigned char ch) {
	return (cmap[ch]&4);//((ch>='a') && (ch<='z')) || ((ch>='A') && (ch<='Z')) || ch=='_';
}
////////////////////////////////////////////////////////////////////////////////
//bool isIDString(const char *s) {
//    if (!isAlpha(*s)){
//        return false;
//    }
//    while (*s) {
//        if (!(isAlpha(*s) || isNumeric(*s))){
//            return false;
//        }
//        s++;
//    }
//    return true;
//}
////////////////////////////////////////////////////////////////////////////////
char* oneLine(char *s, int ptr,int end)
{
	size_t cnt=0;
	static char work[1024];
	if( end < ptr ){
		ptr = end;
	}
	if( ptr>0) ptr--;
	if( ptr>0) ptr--;
	while( ptr > 0  && s[ptr] != '\n'){
		ptr--;
	}
	//ptr++;
	while( s[ptr] && s[ptr] != '\n' && cnt < sizeof( work )-2 ){
		work[cnt] = s[ptr];
		cnt++;
		ptr++;
	}
	work[cnt] = 0;
	return work;
}
////////////////////////////////////////////////////////////////////////////////
/// convert the given std::string into a quoted std::string suitable for javascript
std::string getJSString(const std::string &str) {
	std::string nStr = str;
	for (size_t i=0;i<nStr.size();i++) {
		const char *replaceWith = "";
		bool replace = true;

		switch (nStr[i]) {
			case '\\': replaceWith = "\\\\"; break;
			case '\n': replaceWith = "\\n"; break;
			case '\r': replaceWith = "\\r"; break;
			case '\a': replaceWith = "\\a"; break;
			case '"':  replaceWith = "\\\""; break;
			default: {
								 int nCh = ((int)nStr[i]) &0xFF;
								 if (nCh<32 || nCh>127) {
									 char bytes[5];
									 snprintf(bytes, 5, "\\x%02X", nCh);
									 replaceWith = bytes;
								 } else {
									 replace=false;
								 }
							 }
		}

		if (replace) {
			nStr = nStr.substr(0, i) + replaceWith + nStr.substr(i+1);
			i += strlen(replaceWith)-1;
		}
	}
	return "\"" + nStr + "\"";
}

/** Is the std::string alphanumeric */
bool isAlphaNum(const std::string &str) {
	if (str.size()==0){
		return true;
	}
	if (!isAlpha(str[0])){
		return false;
	}
	for (size_t i=0;i<str.size();i++){
		if (!(isAlpha(str[i]) || isNumeric(str[i]))){
			return false;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------------- CSCRIPTEXCEPTION
CScriptException::CScriptException(const std::string &exceptionText) {
	text = exceptionText;
}

// ----------------------------------------------------------------------------------- CSCRIPTLEX
CScriptLex::CScriptLex(const std::string &input) {
	data = strdup(input.c_str());
	dataOwned = true;
	dataStart = 0;
	dataEnd = strlen(data);
	reset();
}

CScriptLex::CScriptLex(CScriptLex *owner, int startChar, int endChar) {
	data = owner->data;
	dataOwned = false;
	dataStart = startChar;
	dataEnd = endChar;
	reset();
}

CScriptLex::~CScriptLex(void)
{
	if (dataOwned){
		free((void*)data);
	}
}

void CScriptLex::reset() {
	dataPos      = dataStart;
	tokenStart   = 0;
	tokenEnd     = 0;
	tokenLastEnd = 0;
	tk           = LEX_EOF;
	tkStr        = "";
	getNextCh();
	getNextCh();
	getNextToken();
}

void CScriptLex::chkread(int expected_tk) {
	if (tk!=expected_tk) {
		std::stringstream ss;
		ss << "Got " << 
			getTokenStr(tk).c_str() << "expected " << 
			getTokenStr(expected_tk).c_str() << " at " <<
			getPosition(tokenStart).c_str() << "(" << oneLine(data,dataPos,dataEnd) << ")\n";
		throw new CScriptException(ss.str());
	}
	getNextToken();
}

std::string CScriptLex::getTokenStr(int token) {
	if (token>32 && token<128) {
		char buf[4] = "' '";
		buf[1] = (char)token;
		return std::string(buf);
	}
	switch (token) {
		case LEX_EOF            : return "EOF";
		case LEX_ID             : return "ID";
		case LEX_INT            : return "INT";
		case LEX_FLOAT          : return "FLOAT";
		case LEX_STR            : return "STRING";
		case LEX_EQUAL          : return "==";
		case LEX_TYPEEQUAL      : return "===";
		case LEX_NEQUAL         : return "!=";
		case LEX_NTYPEEQUAL     : return "!==";
		case LEX_LEQUAL         : return "<=";
		case LEX_LSHIFT         : return "<<";
		case LEX_LSHIFTEQUAL    : return "<<=";
		case LEX_GEQUAL         : return ">=";
		case LEX_RSHIFT         : return ">>";
		case LEX_RSHIFTUNSIGNED : return ">>";
		case LEX_RSHIFTEQUAL    : return ">>=";
		case LEX_PLUSEQUAL      : return "+=";
		case LEX_MINUSEQUAL     : return "-=";
		case LEX_PLUSPLUS       : return "++";
		case LEX_MINUSMINUS     : return "--";
		case LEX_ANDEQUAL       : return "&=";
		case LEX_ANDAND         : return "&&";
		case LEX_OREQUAL        : return "|=";
		case LEX_OROR           : return "||";
		case LEX_XOREQUAL       : return "^=";
															// reserved words
		case LEX_R_IF           : return "if";
		case LEX_R_ELSE         : return "else";
		case LEX_R_DO           : return "do";
		case LEX_R_WHILE        : return "while";
		case LEX_R_FOR          : return "for";
		case LEX_R_BREAK        : return "break";
		case LEX_R_CONTINUE     : return "continue";
		case LEX_R_FUNCTION     : return "function";
		case LEX_R_RETURN       : return "return";
		case LEX_R_CONST        : return "CONST";
		case LEX_R_VAR          : return "var";
		case LEX_R_TRUE         : return "true";
		case LEX_R_FALSE        : return "false";
		case LEX_R_NULL         : return "null";
		case LEX_R_UNDEFINED    : return "undefined";
		case LEX_R_NEW          : return "new";
		case LEX_R_INCLUDE      : return "include";
	}
	return "?[UNKNOW]";
}

void CScriptLex::getNextCh() {
	currCh = nextCh;
	if (dataPos < dataEnd){
		nextCh = data[dataPos];
	}else{
		nextCh = 0;
	}
	dataPos++;
}

void CScriptLex::getNextToken() {
	tk = LEX_EOF;
	tkStr.clear();

	while (currCh && isWhitespace(currCh)){
		getNextCh();
	}
	// newline comments
	if (currCh=='/' && nextCh=='/') {
		while (currCh && currCh!='\n'){
			getNextCh();
		}
		getNextCh();
		getNextToken();
		return;
	}
	// block comments
	if (currCh=='/' && nextCh=='*') {
		while (currCh && (currCh!='*' || nextCh!='/')) {
			getNextCh();
		}
		getNextCh();
		getNextCh();
		getNextToken();
		return;
	}
	// record beginning of this token(pre-read 2 chars );
	tokenStart = dataPos-2;
	// tokens
	if (isAlpha(currCh)) { //  IDs
		while (isAlpha(currCh) || isNumeric(currCh)) {
			tkStr += currCh;
			getNextCh();
		}
		tk = LEX_ID;
		if (tkStr=="if")        tk = LEX_R_IF;
		else if (tkStr=="else")      tk = LEX_R_ELSE;
		else if (tkStr=="do")        tk = LEX_R_DO;
		else if (tkStr=="while")     tk = LEX_R_WHILE;
		else if (tkStr=="include")   tk = LEX_R_INCLUDE;
		else if (tkStr=="for")       tk = LEX_R_FOR;
		else if (tkStr=="break")     tk = LEX_R_BREAK;
		else if (tkStr=="continue")  tk = LEX_R_CONTINUE;
		else if (tkStr=="function")  tk = LEX_R_FUNCTION;
		else if (tkStr=="return")    tk = LEX_R_RETURN;
		else if (tkStr=="var")       tk = LEX_R_VAR;
		else if (tkStr=="const")     tk = LEX_R_CONST;
		else if (tkStr=="true")      tk = LEX_R_TRUE;
		else if (tkStr=="false")     tk = LEX_R_FALSE;
		else if (tkStr=="null")      tk = LEX_R_NULL;
		else if (tkStr=="undefined") tk = LEX_R_UNDEFINED;
		else if (tkStr=="new")       tk = LEX_R_NEW;
	} else if (isNumeric(currCh)) { // Numbers
		bool isHex = false;
		if (currCh=='0') { tkStr += currCh; getNextCh(); }
		if (currCh=='x') {
			isHex = true;
			tkStr += currCh; getNextCh();
		}
		tk = LEX_INT;
		while (isNumeric(currCh) || (isHex && isHexadecimal(currCh))) {
			tkStr += currCh;
			getNextCh();
		}
		if (!isHex && currCh=='.') {
			tk = LEX_FLOAT;
			tkStr += '.';
			getNextCh();
			while (isNumeric(currCh)) {
				tkStr += currCh;
				getNextCh();
			}
		}
		// do fancy e-style floating point
		if (!isHex && (currCh=='e'||currCh=='E')) {
			tk = LEX_FLOAT;
			tkStr += currCh;
			getNextCh();
			if (currCh=='-') {
				tkStr += currCh;
				getNextCh();
			}
			while (isNumeric(currCh)) {
				tkStr += currCh;
				getNextCh();
			}
		}
	} else if (currCh=='"') {
		// strings...
		getNextCh();
		while (currCh && currCh!='"') {
			if (currCh == '\\') {
				getNextCh();
				switch (currCh) {
					case 'n' : tkStr += '\n'; break;
					case 'r' : tkStr += '\r'; break;
					case 't' : tkStr += '\t'; break;
					case '"' : tkStr += '"'; break;
					case '\\' : tkStr += '\\'; break;
					default: tkStr += currCh;
				}
			} else {
				tkStr += currCh;
			}
			getNextCh();
		}
		getNextCh();
		tk = LEX_STR;
	} else if (currCh=='\'') {
		// strings again...
		getNextCh();
		while (currCh && currCh!='\'') {
			if (currCh == '\\') {
				getNextCh();
				switch (currCh) {
					case 'n' : tkStr += '\n'; break;
					case 'a' : tkStr += '\a'; break;
					case 'r' : tkStr += '\r'; break;
					case 't' : tkStr += '\t'; break;
					case '\'' : tkStr += '\''; break;
					case '\\' : tkStr += '\\'; break;
					case 'x' : { // hex digits
											 char buf[3] = "??";
											 getNextCh(); buf[0] = currCh;
											 getNextCh(); buf[1] = currCh;
											 tkStr += (char)strtol(buf,0,16);
										 } break;
					default: if (currCh>='0' && currCh<='7') {
										 // octal digits
										 char buf[4] = "???";
										 buf[0] = currCh;
										 getNextCh(); buf[1] = currCh;
										 getNextCh(); buf[2] = currCh;
										 tkStr += (char)strtol(buf,0,8);
									 } else
										 tkStr += currCh;
				}
			} else {
				tkStr += currCh;
			}
			getNextCh();
		}
		getNextCh();
		tk = LEX_STR;
	} else {
		// single chars
		tk = (LEX_TYPES)currCh;
		if (currCh) getNextCh();
		if (tk=='=' && currCh=='=') { // ==
			tk = LEX_EQUAL;
			getNextCh();
			if (currCh=='=') { // ===
				tk = LEX_TYPEEQUAL;
				getNextCh();
			}
		} else if (tk=='!' && currCh=='=') { // !=
			tk = LEX_NEQUAL;
			getNextCh();
			if (currCh=='=') { // !==
				tk = LEX_NTYPEEQUAL;
				getNextCh();
			}
		} else if (tk=='<' && currCh=='=') {
			tk = LEX_LEQUAL;
			getNextCh();
		} else if (tk=='<' && currCh=='<') {
			tk = LEX_LSHIFT;
			getNextCh();
			if (currCh=='=') { // <<=
				tk = LEX_LSHIFTEQUAL;
				getNextCh();
			}
		} else if (tk=='>' && currCh=='=') {
			tk = LEX_GEQUAL;
			getNextCh();
		} else if (tk=='>' && currCh=='>') {
			tk = LEX_RSHIFT;
			getNextCh();
			if (currCh=='=') { // >>=
				tk = LEX_RSHIFTEQUAL;
				getNextCh();
			} else if (currCh=='>') { // >>>
				tk = LEX_RSHIFTUNSIGNED;
				getNextCh();
			}
		}  else if (tk=='+' && currCh=='=') {
			tk = LEX_PLUSEQUAL;
			getNextCh();
		}  else if (tk=='-' && currCh=='=') {
			tk = LEX_MINUSEQUAL;
			getNextCh();
		}  else if (tk=='+' && currCh=='+') {
			tk = LEX_PLUSPLUS;
			getNextCh();
		}  else if (tk=='-' && currCh=='-') {
			tk = LEX_MINUSMINUS;
			getNextCh();
		} else if (tk=='&' && currCh=='=') {
			tk = LEX_ANDEQUAL;
			getNextCh();
		} else if (tk=='&' && currCh=='&') {
			tk = LEX_ANDAND;
			getNextCh();
		} else if (tk=='|' && currCh=='=') {
			tk = LEX_OREQUAL;
			getNextCh();
		} else if (tk=='|' && currCh=='|') {
			tk = LEX_OROR;
			getNextCh();
		} else if (tk=='^' && currCh=='=') {
			tk = LEX_XOREQUAL;
			getNextCh();
		}
	}
	/* This isn't quite right yet */
	tokenLastEnd = tokenEnd;
	tokenEnd = dataPos-3;
}

std::string CScriptLex::getSubString(int lastPosition) {
	int lastCharIdx = tokenLastEnd+1;
	if (lastCharIdx < dataEnd) {
		/* save a memory alloc by using our data array to create the
			 subistring */
		char old = data[lastCharIdx];
		data[lastCharIdx] = 0;
		std::string value = &data[lastPosition];
		data[lastCharIdx] = old;
		return value;
	} else {
		return std::string(&data[lastPosition]);
	}
}

CScriptLex *CScriptLex::getSubLex(int lastPosition) {
	int lastCharIdx = tokenLastEnd+1;
	if (lastCharIdx < dataEnd)
		return new CScriptLex(this, lastPosition, lastCharIdx);
	else
		return new CScriptLex(this, lastPosition, dataEnd );
}

std::string CScriptLex::getPosition(int pos) {
	if (pos<0) pos=tokenLastEnd;
	int line = 1;
	int col  = 1;
	for (int i=0;i<pos;i++) {
		char ch;
		if (i < dataEnd){
			ch = data[i];
		}else{
			ch = 0;
		}
		col++;
		if (ch=='\n') {
			line++;
			col = 1;
		}
	}
	std::stringstream ss;
	ss << "(line: " << line << ", col: " << col << ")";
	return ss.str();
}

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
void Compiler::run(const std::string &fname) {
	std::string oldCwd = cwd;

	cname = File::getFullname(cwd, fname);
	cwd = File::getPath(cname);

	//	TRACE("Runing file \"%s\n", cname.c_str());
	std::string input = File::read(cname);
	if(input.length() > 0) {
		exec(input);
	}
	else {
		//		TRACE("Can not run file \"%s\" at \"%s\" %s.\n", fname.c_str(), cname.c_str(), l->getPosition().c_str());
	}

	if(oldCwd.length() > 0)
		cwd = oldCwd;
}

void Compiler::exec(const std::string &code) {
	CScriptLex* old = l;

	l = new CScriptLex(code);

	try {
		while (l->tk) {
			statement();
		}
		bytecode.reserve(); //add a nil instruction at the end of bytecode.
	} catch (CScriptException *e) {
		std::string msg;
		std::stringstream ss;
		ss << "Error at " << l->getPosition().c_str() << ": " << e->text.c_str();
		//msg +=  e->text;
		msg = ss.str();

		delete l;
		delete e;

		throw new CScriptException(msg.c_str());
	}
	delete l;
	l = old;
}

LEX_TYPES Compiler::statement(bool pop) {
	LEX_TYPES ret = LEX_EOF;

	if (l->tk==LEX_R_INCLUDE) {
		l->chkread(LEX_R_INCLUDE);

		run(l->tkStr);

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
		PC pc = bytecode.reserve();
		Loop* l = getLoop();
		if(l != NULL)
			l->breaks.push_back(pc);
	} 
	else if (l->tk==LEX_R_CONTINUE){
		l->chkread(LEX_R_CONTINUE);
		l->chkread(';');
		//reserve for jump to begin of loop condition.
		PC pc = bytecode.reserve();
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
			bytecode.gen(beConst ? INSTR_CONST : INSTR_VAR, vname);
			// sort out initialiser
			if (l->tk == '=') {
				l->chkread('=');
				bytecode.gen(INSTR_LOAD, vname);
				base();
				bytecode.gen(INSTR_ASIGN);
				bytecode.gen(INSTR_POP);
			}

			if (l->tk != ';')
				l->chkread(',');
		}      
		l->chkread(';');
	}
	else if(l->tk==LEX_R_FUNCTION) {
		defFunc();
	}
	else if (l->tk==LEX_R_RETURN) {
		l->chkread(LEX_R_RETURN);
		pop = false;
		if (l->tk != ';') {
			base();
			bytecode.gen(INSTR_RETURNV);
		}
		else {
			bytecode.gen(INSTR_RETURN);
		}
		l->chkread(';');
	} 
	else if (l->tk==LEX_R_WHILE) {
		Loop loop;
		loopStack.push(loop);

		l->chkread(LEX_R_WHILE);
		l->chkread('(');
		PC cpc = bytecode.getPC();
		base(); //condition
		l->chkread(')');
		PC pc = bytecode.reserve();
		statement(false);
		bytecode.addJmp(cpc, INSTR_JMPB); //coninue anchor;
		bytecode.setJmp(pc, INSTR_NJMP); // end anchor;

		setLoopBreaks(getLoop(), bytecode.getPC());
		setLoopContinues(getLoop(), cpc);
		loopStack.pop();
	}
	else if (l->tk==LEX_R_IF) {
		l->chkread(LEX_R_IF);
		l->chkread('(');
		base(); //condition
		l->chkread(')');
		PC pc = bytecode.reserve();
		statement(false);

		if (l->tk==LEX_R_ELSE) {
			l->chkread(LEX_R_ELSE);
			PC pc2 = bytecode.reserve();
			bytecode.setJmp(pc, INSTR_NJMP);
			statement(false);
			bytecode.setJmp(pc2, INSTR_JMP);
		}
		else {
			bytecode.setJmp(pc, INSTR_NJMP);
		}
	}
	else if (l->tk==LEX_R_FOR) {
		Loop loop;
		loopStack.push(loop);

		l->chkread(LEX_R_FOR);
		l->chkread('(');
		statement(); //init
		//l->chkread(';');
		PC cpc = bytecode.getPC();
		base(); //continue
		PC breakPC = bytecode.reserve();
		PC loopPC = bytecode.reserve();
		l->chkread(';');
		PC ipc = bytecode.getPC();
		base(); //iterator
		bytecode.gen(INSTR_POP);
		l->chkread(')');
		bytecode.addJmp(cpc, INSTR_JMPB); //continue anchor
		bytecode.setJmp(loopPC, INSTR_JMP);
		statement(false);
		bytecode.addJmp(ipc, INSTR_JMPB); //iterator anchor
		bytecode.setJmp(breakPC, INSTR_NJMP); //end anchor
		
		setLoopBreaks(getLoop(), bytecode.getPC());
		setLoopContinues(getLoop(), cpc);
		loopStack.pop();
	}
	else {
		l->chkread(LEX_EOF);
	}

	if(pop)
		bytecode.gen(INSTR_POP);
	return ret;
}

LEX_TYPES Compiler::base() {
	LEX_TYPES ret = LEX_EOF;
	ret = ternary();
	if (l->tk=='=' || l->tk==LEX_PLUSEQUAL || l->tk==LEX_MINUSEQUAL) {
		// sort out initialiser
		if (l->tk == '=') {
			l->chkread('=');
			base();
			bytecode.gen(INSTR_ASIGN);
		}
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
		bytecode.gen(INSTR_NOT);
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
			bytecode.gen(INSTR_MULTI);
		else if(op == '/')
			bytecode.gen(INSTR_DIV);
		else
			bytecode.gen(INSTR_MOD);
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
		bytecode.gen(INSTR_NEG);
	}
	else if(pre==LEX_PLUSPLUS) {
		bytecode.gen(INSTR_PPLUS);
	}
	else if(pre==LEX_MINUSMINUS) {
		bytecode.gen(INSTR_MMINUS);
	}

	while (l->tk=='+' || l->tk=='-' ||
			l->tk==LEX_PLUSPLUS || l->tk==LEX_MINUSMINUS) {
		int op = l->tk;
		l->chkread(l->tk);
		if (op==LEX_PLUSPLUS) {
			bytecode.gen(INSTR_PPLUS);
		}
		else if(op==LEX_MINUSMINUS) {
			bytecode.gen(INSTR_MMINUS);
		}
		else {
			ret = term();
			if(op== '+') {
				bytecode.gen(INSTR_PLUS);
			}
			else if(op=='-') {
				bytecode.gen(INSTR_MINUS);
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
			bytecode.gen(INSTR_LSHIFT);
		else if (op==LEX_RSHIFT)
			bytecode.gen(INSTR_RSHIFT);
		else
			bytecode.gen(INSTR_URSHIFT);
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
			bytecode.gen(INSTR_EQ);
		else if(op == LEX_NEQUAL)
			bytecode.gen(INSTR_NEQ);
		else if(op == LEX_LEQUAL)
			bytecode.gen(INSTR_LEQ);
		else if(op == LEX_GEQUAL)
			bytecode.gen(INSTR_GEQ);
		else if(op == '>')
			bytecode.gen(INSTR_GRT);
		else if(op == '<')
			bytecode.gen(INSTR_LES);
	}

	return ret;	
}

LEX_TYPES Compiler::logic() {
	LEX_TYPES ret = LEX_EOF;
	ret = condition();

	while (l->tk=='&' || l->tk=='|' || l->tk=='^' || l->tk==LEX_ANDAND || l->tk==LEX_OROR) {
		ret = condition();

		int op = l->tk;
		l->chkread(l->tk);
		if (op==LEX_ANDAND) {
			bytecode.gen(INSTR_AAND);
		} 
		else if (op==LEX_OROR) {
			bytecode.gen(INSTR_OOR);
		}
		else if (op=='|') {
			bytecode.gen(INSTR_OR);
		}
		else if (op=='&') {
			bytecode.gen(INSTR_AND);
		}
		else if (op=='^') {
			bytecode.gen(INSTR_XOR);
		}
	}
	return ret;	
}

LEX_TYPES Compiler::ternary() {
	LEX_TYPES ret = LEX_EOF;
	ret = logic();
	
	if (l->tk=='?') {
		PC pc1 = bytecode.reserve(); //keep for jump
		l->chkread('?');
		base(); //first choice
		PC pc2 = bytecode.reserve(); //keep for jump
		l->chkread(':');
		bytecode.setJmp(pc1, INSTR_NJMP);
		base(); //second choice
		bytecode.setJmp(pc2, INSTR_JMP);
	} 
	return ret;	
}

LEX_TYPES Compiler::callFunc() {
	LEX_TYPES ret = LEX_EOF;
	l->chkread('(');

	while(true) {
		base();
		if (l->tk!=')')
			l->chkread(',');	
		else
			break;
	}
	l->chkread(')');
	return ret;
}

LEX_TYPES Compiler::defFunc() {
	LEX_TYPES ret = LEX_EOF;
	// actually parse a function...
	l->chkread(LEX_R_FUNCTION);
	std::string funcName = "";
	/* we can have functions without names */
	if (l->tk==LEX_ID) {
		funcName = l->tkStr;
		l->chkread(LEX_ID);
	}

	bytecode.gen(INSTR_FUNC, funcName.c_str());
	//do arguments
	l->chkread('(');
	while (l->tk!=')') {
		bytecode.gen(INSTR_ARG, l->tkStr.c_str());
		l->chkread(LEX_ID);
		if (l->tk!=')') l->chkread(',');
	}
	l->chkread(')');
	PC pc = bytecode.reserve();

	int funcBegin = l->tokenStart;
	block();
	
	OpCode op = bytecode.getOpCode(bytecode.getPC() - 1) >> 16;

	if(op != INSTR_RETURN && op != INSTR_RETURNV)
		bytecode.gen(INSTR_RETURN);
	bytecode.setJmp(pc, INSTR_FUNC_END);
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
		bytecode.gen(INSTR_TRUE);
	}
	else if (l->tk==LEX_R_FALSE) {
		l->chkread(LEX_R_FALSE);
		bytecode.gen(INSTR_FALSE);
	}
	else if (l->tk==LEX_R_NULL) {
		l->chkread(LEX_R_NULL);
		bytecode.gen(INSTR_NULL);
	}
	else if (l->tk==LEX_R_UNDEFINED) {
		l->chkread(LEX_R_UNDEFINED);
		bytecode.gen(INSTR_UNDEF);
	}
	else if (l->tk==LEX_INT) {
		bytecode.gen(INSTR_INT, l->tkStr);
		l->chkread(LEX_INT);
	}
	else if (l->tk==LEX_FLOAT) {
		bytecode.gen(INSTR_FLOAT, l->tkStr);
		l->chkread(LEX_FLOAT);
	}
	else if (l->tk==LEX_STR) {
		bytecode.gen(INSTR_STR, l->tkStr);
		l->chkread(LEX_STR);
	}
	else if(l->tk==LEX_R_FUNCTION) {
		defFunc();
	}
	else if (l->tk==LEX_R_NEW) {
		// new -> create a new object
		l->chkread(LEX_R_NEW);
		const std::string className = l->tkStr;
		l->chkread(LEX_ID);
		if (l->tk == '(') {
			l->chkread('(');
			l->chkread(')');
			bytecode.gen(INSTR_NEW, className);
		}
	}
	if (l->tk=='{') {
		/* JSON-style object definition */
		l->chkread('{');
		bytecode.gen(INSTR_OBJ);
		while (l->tk != '}') {
			string id = l->tkStr;
			bytecode.gen(INSTR_STR, id);
			// we only allow strings or IDs on the left hand side of an initialisation
			if (l->tk==LEX_STR) l->chkread(LEX_STR);
			else l->chkread(LEX_ID);
			l->chkread(':');
			base();
			bytecode.gen(INSTR_MEMBER);
			// no need to clean here, as it will definitely be used
			if (l->tk != '}') l->chkread(',');
		}
		bytecode.gen(INSTR_OBJ_END);
		l->chkread('}');
	}

	else if(l->tk==LEX_ID) {
		string name = l->tkStr;
		l->chkread(LEX_ID);

		bool load = true;
		vector<string> names;
		while (l->tk=='(' || l->tk=='.' || l->tk=='[') {
			if (l->tk=='(') { // ------------------------------------- Function Call
				callFunc();
				StringUtil::split(name, '.', names);
				name = "";
				int sz = names.size()-1;
					
				if(sz == 0 && load) {
					bytecode.gen(INSTR_LOAD, "this");	
				}
				else {
					for(int i=0; i<sz; i++) {
						bytecode.gen(load ? INSTR_LOAD:INSTR_GET, names[i]);	
						load = false;
					}
				}
				bytecode.gen(INSTR_CALL, names[sz]);	
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
				int sz = names.size();
				for(int i=0; i<sz; i++) {
					bytecode.gen(load ? INSTR_LOAD:INSTR_GET, names[i]);	
					load = false;
				}

				l->chkread('[');
				base();
				l->chkread(']');
				bytecode.gen(INSTR_ARRAY_AT);
			} 
		}
		if(name.length() > 0) {
			StringUtil::split(name, '.', names);
			name = "";
			int sz = names.size();
			for(int i=0; i<sz; i++) {
				bytecode.gen(load ? INSTR_LOAD:INSTR_GET, names[i]);	
				load = false;
			}
		}
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
		for(int i=0; i<loop->breaks.size(); ++i) {
			bytecode.setJmp(loop->breaks[i], INSTR_JMP, pc);
		}
	}
}

void Compiler::setLoopContinues(Loop* loop, PC pc) {
	if(loop != NULL) {
		for(int i=0; i<loop->continues.size(); ++i) {
			bytecode.setJmp(loop->continues[i], INSTR_JMPB, pc);
		}
	}
}
