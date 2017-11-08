#include "KoalaJS.h"
#include "Compiler.h"
#include "utils/String/StringUtil.h"
#include "utils/File/File.h"
#include "utils/Thread/Thread.h"
#include <sstream>  
#include <stdlib.h>
#include <stdarg.h>

#define INTERUPT_COUNT 10240

static GlobalVars _globalVars;

GlobalVars* KoalaJS::getGlobalVars() {
	return &_globalVars;
}

static ThreadLock _intrLock;

void KoalaJS::doInterupt() {
	_intrLock.lock();
	Interupt* intr = NULL;
	if(!interupts.empty()) {
		intr = interupts.front();
		interupts.pop();
	}
	size_t sz = interupts.size();
	_intrLock.unlock();

	if(intr == NULL)
		return;

	TRACE("Interupted for '%s$%d', %d in queue.\n", 
			intr->funcName.c_str(), 
			(int)intr->args.size(),
			(int)sz);

	BCVar* v = callJSFunc(intr->funcName, intr->args);
	v->unref();
	delete intr;
}

void KoalaJS::interupt(const string& funcName, int argNum, ...) {
	Interupt *intr = new Interupt(funcName);

	va_list args;
	va_start(args, argNum);
	for(int i=0; i<argNum; ++i) {
		BCVar* v = va_arg(args, BCVar*);
		intr->args.push_back(v);
	}
	va_end(args);

	_intrLock.lock();
	interupts.push(intr);
	_intrLock.unlock();
}

BCVar* KoalaJS::callJSFunc(const string& funcName, const vector<BCVar*>& args) {
	BCVar* v = NULL;
	KoalaJS js(getRoot());
	js.moduleLoader = getModuleLoader();
	js.setBytecode(bcode);

	size_t argNum = args.size();
	for(size_t i=0; i<argNum; ++i) {
		v = args[i];
		js.push(v->ref());
	}

	js.push(js.root->ref());

	string fname = funcName;
	if(argNum > 0) {
		fname = fname + "$" + StringUtil::from((int)argNum);
	}

	VMScope sc;
	sc.var = js.root->ref();
	sc.pc = 0;
	js.pushScope(sc);

	size_t scDeep = js.scopes.size();
	js.funcCall(fname, false);
	if(scDeep < js.scopes.size()) {
		js.runCode(NULL);
	}
	js.popScope();

	StackItem* i = js.pop2();
	if(i != NULL)
		v = VAR(i);
	else {
		v = new BCVar();
		v->ref();
	}
	return v;
}

BCVar* KoalaJS::callJSFunc(const string& funcName, int argNum, ...) {
	BCVar* v = NULL;
	vector<BCVar*> params;

	va_list args;
	va_start(args, argNum);
	for(int i=0; i<argNum; ++i) {
		v = va_arg(args, BCVar*);
		params.push_back(v);
	}
	va_end(args);

	return callJSFunc(funcName, params);
}


BCVar* KoalaJS::newObject(const string& clsName) {
	if(clsName.length() == 0)
		return NULL;

	BCVar* ret = NULL;
	if(clsName == CLS_ARR) {
		ret = new BCVar();
		ret->type = BCVar::ARRAY;
		return ret;
	}
	if(clsName == CLS_OBJECT) {
		ret = new BCVar();
		ret->type = BCVar::OBJECT;
		return ret;
	}

	BCNode* cls = findInScopes(clsName);
	if(cls == NULL) {
		ERR("Class %s not found\n", clsName.c_str());
		return NULL;
	}

	return newObject(cls);
}

BCVar* KoalaJS::newObject(BCNode* cls) {
	BCVar* ret = NULL;
	if(cls->var->type != BCVar::CLASS) {
		ERR("%s is not a class\n", cls->name.c_str());
		return NULL;
	}

	ret = new BCVar();
	ret->type = BCVar::OBJECT;
	ret->addChild(PROTOTYPE, cls->var);
	return ret;
}

void KoalaJS::run(const std::string &fname) {
	std::string oldCwd = cwd;
	cname = File::getFullname(cwd, fname);
	cwd = File::getPath(cname);

	Bytecode* bc = CodeCache::get(cname);
	if(bc != NULL) {
		runCode(bc);
	}
	else {
		bc = new Bytecode();
		if(cname.find(".bcode") != string::npos) {
			if(bc->fromFile(cname)) {
				CodeCache::cache(cname, bc);
				runCode(bc);
			}
		}
		else {
			Compiler compiler;
			if(compiler.run(cname)) {
				//			compiler.bytecode.dump();
				compiler.bytecode.clone(bc);
				CodeCache::cache(cname, bc);
				runCode(bc);
			}
		}
	}

	if(oldCwd.length() > 0)
		cwd = oldCwd;
}

void KoalaJS::exec(const std::string &code) {
	Bytecode* bc = CodeCache::get(code);
	if(bc == NULL) {
		bc = new Bytecode();
		Compiler compiler;
		if(compiler.exec(code)) {
			compiler.bytecode.clone(bc);
			CodeCache::cache(code, bc);
		}
	}
	runCode(bc);
}


StackItem* KoalaJS::pop2() {
	if(stackTop == STACK_DEEP) // touch the bottom of stack
		return NULL;

	StackItem* ret =  vStack[stackTop];
	vStack[stackTop] = NULL;
	stackTop++;
	return ret;
}

void KoalaJS::pop() {
	if(stackTop == STACK_DEEP)
		return;

	StackItem* i =  vStack[stackTop];
	vStack[stackTop] = NULL;
	stackTop++;
	VAR(i)->unref();
}

void KoalaJS::push(StackItem* v) {
	if(stackTop == 0) { //stack overflow
		ERR("stack overflow\n");
		return;
	}
	vStack[--stackTop] = v;
}

BCNode* KoalaJS::find(const string& name) {
	VMScope* sc = scope();
	if(sc == NULL)
		return NULL;

	return sc->var->getChild(name);
}

BCNode* KoalaJS::findInScopes(const string& name) {
	for(int i=(int)scopes.size() - 1; i >= 0; --i) {
		BCNode* r = scopes[i].var->getChild(name);
		if(r != NULL)
			return r;
	}
	return NULL;
}

BCNode* KoalaJS::findInClass(BCVar* obj, const string& name) {
	BCNode* n = obj->getChild(name);
	if(n != NULL)
		return n;

	if(obj->type == BCVar::STRING) {
		BCNode* cls = findInScopes("String");
		if(cls != NULL)	
			return cls->var->getChild(name);
	}
	else if(obj->type == BCVar::ARRAY) {
		BCNode* cls = findInScopes("Array");
		if(cls != NULL)	
			return cls->var->getChild(name);
	}

	while(obj != NULL) {
		BCNode* n;
		n = obj->getChild(PROTOTYPE);
		if(n != NULL) {
			obj = n->var;
			n = obj->getChild(name);
			if(n != NULL)
				return n;
		}		
		else {
			break;
		}
	}
	return NULL;
}

BCVar* KoalaJS::getCurrentObj(bool create) {
	BCNode* n = findInScopes(THIS);
	if(n != NULL)
		return n->var;

	BCVar* ret = NULL;
	if(create) {
		ret = new BCVar();
		ret->type = BCVar::OBJECT;
	}
	return ret;
}

bool KoalaJS::construct(BCVar* obj, int argNum) {
	push(obj->ref());
	string fname = CONSTRUCTOR;
	fname = fname + "$" + StringUtil::from(argNum);
	if(!funcCall(fname, true)) {
		obj = (BCVar*)pop2();
		obj->unref(false);
		return false;
	}
	return true;
}

void KoalaJS::doNew(const string& clsName) { //TODO: construct with arguments.
	BCVar* ret = NULL;

	size_t pos = clsName.find("$");
	string cn = clsName;
	int argNum = 0;
	if(pos != string::npos)	{
		cn = clsName.substr(0, pos);
		string argS = clsName.substr(pos + 1);
		argNum = atoi(argS.c_str());
	}

	if(cn.length() == 0) {
		ret = new BCVar();	
		ret->type = BCVar::OBJECT;
	}	
	else if(cn == CLS_ARR) {
		ret = new BCVar();
		ret->type = BCVar::ARRAY;
	}
	else if(cn == CLS_OBJECT) {
		ret = new BCVar();
		ret->type = BCVar::OBJECT;
	}
	else {
		BCNode* cls = findInScopes(clsName);
		if(cls == NULL && pos != string::npos)
			cls = findInScopes(cn);

		if(cls == NULL) {
			ERR("Class %s not found\n", cn.c_str());
			return;
		}
		if(cls->var->isFunction()) {
			BCVar* v = getCurrentObj(true);
			push(v->ref());	//push this
			if(!funcCall(clsName))
				pop(); //pop and drop this
			return;
		}
		ret = newObject(cls);
		if(construct(ret, argNum))
			return;
	}

	if(ret != NULL)
		push(ret->ref());
}

BCNode* KoalaJS::findFunc(BCVar* owner, const string& fname, bool member) {
	//find function in object;
	BCNode*	n = owner->getChild(fname);
	if(n == NULL && member)
		n = findInClass(owner, fname);

	//find function in scopes;
	if(n == NULL)
		n = findInScopes(fname);

	if(n == NULL) {
		return NULL;
	}

	if(!n->var->isFunction()) {
		return NULL;
	}
	return n;
}

bool KoalaJS::funcCall(const string& funcName, bool member) {
	if(funcName.length() == 0)
		return false;

	size_t pos = funcName.find("$");
	string fname = funcName;
	if(pos != string::npos)	{
		fname = funcName.substr(0, pos);
	}

	//read object
	StackItem* si = pop2();
	BCVar* object = NULL;
	if(si == NULL)  {
		return false;
	}
	object = VAR(si);	

	BCNode* n = findFunc(object, funcName, member);
	if(n == NULL) {
		if(pos != string::npos)
			n = findFunc(object, fname, member);

		if(n == NULL) {
			if(fname != CONSTRUCTOR)
				ERR("Function '%s' not found\n", fname.c_str());
			push(object); //push back to stack
			return false;
		}
	}

	BCVar* params = new BCVar(); //function parameters
	FuncT* func = n->var->getFunc();
	params->addChild(THIS, object);
	object->unref(); //unref after pop

	//read arguments
	int argNum = (int)func->args.size();
	for(int i=argNum-1; i>=0; --i) {
		string arg = func->args[i];
		si = pop2();
		if(si == NULL) {
			ERR("%s argument not match\n", fname.c_str());
			delete params;
			return false;
		}
		BCVar* v = VAR(si);
		params->addChild(arg, v);
		v->unref(); //unref after pop
	}

	VMScope sc;
	sc.var = params->ref();

	if(n->var->type == BCVar::NFUNC) { //native function
		if(func->native != NULL) {
			pushScope(sc);
			func->native(this, params, func->data);
			//read return.
			BCVar* ret = params->getReturnVar();
			push(ret->ref());
			popScope();
		}
		return true;
	}

	sc.pc = pc;
	pushScope(sc);
	//js function
	pc = func->pc;
	return true;
}

BCVar* KoalaJS::funcDef(const string& funcName, bool regular) {
	BCVar* ret = NULL;
	vector<string> args;

	//check redefined.
	if(funcName.length() > 0) {
		BCNode* n = findInScopes(funcName);
		if(n != NULL) {
			ERR("Function '%s' has already been defined\n", funcName.c_str());			
			return NULL;
		}
	}
	//read arguments
	PC funcPC = 0;
	while(true) {
		PC ins = code[pc++];
		OpCode instr = ins >> 16;
		OpCode offset = ins & 0x0000FFFF;

		if(instr == INSTR_JMP) {
			funcPC = pc;
			pc = pc + offset - 1;
			break;
		}
		args.push_back(bcode->getStr(offset));
	}
	//create function variable
	ret = new BCVar();
	ret->setFunction(funcPC);
	ret->getFunc()->regular = regular;
	ret->getFunc()->args = args;
	return ret;
}

BCVar* KoalaJS::getOrAddClass(const string& clsName) {
	BCNode* cls = root->getChildOrCreate(clsName);
	if(cls == NULL)
		return NULL;
	cls->var->type = BCVar::CLASS;
	return cls->var;
}

void KoalaJS::addNative(const string& clsName, const string& funcDecl, JSCallback native, void* data) {
	BCVar* clsVar = NULL;
	if(clsName.length() == 0) {
		clsVar = root;
	}
	else {
		clsVar = getOrAddClass(clsName);
	}

	size_t i = funcDecl.find('(');
	if(i == string::npos) {
		ERR("Register native function '(' missed\n");	
		return;
	}
	//read func name	
	string funcName = funcDecl.substr(0, i);
	if(funcName.length() == 0) {	
		ERR("Register native function name missed\n");	
		return;
	}

	//read func args
	string s = funcDecl.substr(i+1);
	i = s.rfind(')');
	if(i == string::npos) {
		ERR("Register native function ')' missed\n");	
		return;
	}
	s = s.substr(0, i);

	vector<string> args;
	while(true) {
		string arg;
		i = s.find(',');
		if(i != string::npos) {
			arg = s.substr(0, i);
			s = s.substr(i+1);
		}
		else {
			arg = s;
			s = "";
		}

		arg = StringUtil::trim(arg);
		if(arg.length() == 0) 
			break;
		args.push_back(arg);
	}

	BCVar* funcVar = new BCVar();
	size_t argNum = args.size();
	funcVar->setFunction(0, native);
	FuncT* func = funcVar->getFunc();
	func->data = data;
	func->args = args;

	if(argNum > 0)
		funcName = funcName + "$" + StringUtil::from((int)argNum);	
	clsVar->addChild(funcName, funcVar);
}

void KoalaJS::init(BCVar* rt) {
	if(rt == NULL) {
		root = new BCVar();
		root->type = BCVar::OBJECT;
	}
	else
		root = rt;

	root->ref();
}

void KoalaJS::compare(OpCode op, BCVar* v1, BCVar* v2) {
	float f1, f2;
	f1 = v1->getFloat();
	f2 = v2->getFloat();

	bool i = false;
	if(v1->type == v2->type) {
		if(v1->isUndefined()) {
			switch(op) {
				case INSTR_EQ: 
					i = v2->isUndefined();
					break; 
				case INSTR_NEQ: 
					i = !v2->isUndefined();
					break;
			}
		}
		else if(v1->isString()) {
			switch(op) {
				case INSTR_EQ: 
					i = (v1->getString() == v2->getString());
					break; 
				case INSTR_NEQ: 
					i = (v1->getString() != v2->getString());
					break;
			}
		}
		else {
			switch(op) {
				case INSTR_EQ: 
					i = (f1 == f2);
					break; 
				case INSTR_NEQ: 
					i = (f1 != f2);
					break; 
				case INSTR_LES: 
					i = (f1 < f2);
					break; 
				case INSTR_GRT: 
					i = (f1 > f2);
					break; 
				case INSTR_LEQ: 
					i = (f1 <= f2);
					break; 
				case INSTR_GEQ: 
					i = (f1 >= f2);
					break; 
			}
		}
	}
	else if(op == INSTR_NEQ) {
		i = true;
	}

	BCVar* v = new BCVar(i ? 1 : 0);
	push(v->ref());
}

void KoalaJS::mathOp(OpCode op, BCVar* v1, BCVar* v2) {
	if(v1->isNumber() && v2->isNumber()) {
		//do number 
		float f1, f2, ret = 0.0;
		bool floatMode = false;
		if(v1->type == BCVar::FLOAT || v2->type == BCVar::FLOAT)
			floatMode = true;

		f1 = v1->getFloat();
		f2 = v2->getFloat();


		switch(op) {
			case INSTR_PLUS: 
			case INSTR_PLUSEQ: 
				ret = (f1 + f2);
				break; 
			case INSTR_MINUS: 
			case INSTR_MINUSEQ: 
				ret = (f1 - f2);
				break; 
			case INSTR_DIV: 
			case INSTR_DIVEQ: 
				ret = (f1 / f2);
				break; 
			case INSTR_MULTI: 
			case INSTR_MULTIEQ: 
				ret = (f1 * f2);
				break; 
			case INSTR_MOD: 
			case INSTR_MODEQ: 
				ret = (((int)f1) % (int)f2);
				break; 
		}

		BCVar* v;
		if(op == INSTR_PLUSEQ || 
				op == INSTR_MINUSEQ ||
				op == INSTR_DIVEQ ||
				op == INSTR_MULTIEQ ||
				op == INSTR_MODEQ) 
			v = v1;
		else
			v = new BCVar();

		if(floatMode) {
			v->setFloat(ret);
		}
		else {
			v->setInt((int)ret);
		}
		push(v->ref());
		return;
	}

	//do string + 
	if(op == INSTR_PLUS || op == INSTR_PLUSEQ) {
		string s = v1->getString();
		ostringstream ostr;  
		switch(v2->type) {
			case BCVar::STRING: {
														ostr << s << v2->getString();
														break;
													}
			case BCVar::INT: {
												 ostr << s << v2->getInt();
												 break;
											 }
			case BCVar::FLOAT: {
													 ostr << s << v2->getFloat();
													 break;
												 }
			case BCVar::ARRAY: 
			case BCVar::OBJECT: {
														ostr << s << v2->getJSON();
														break;
													}
		}

		s = ostr.str();
		BCVar* v;
		if(op == INSTR_PLUSEQ || op == INSTR_MINUSEQ) 
			v = v1;
		else
			v = new BCVar();
		v->setString(s);
		push(v->ref());
	}
}

void KoalaJS::doGet(BCVar* v, const string& str) {
	if(v->isString() && str == "length") {
		BCVar* i = new BCVar((int)v->getString().length());
		push(i->ref());
		return;
	}
	else if(v->isArray() && str == "length") {
		BCVar* i = new BCVar(v->getChildrenNum());
		push(i->ref());
		return;
	}	

	BCNode* n = v->getChild(str);
	if(n == NULL)
		n = findInClass(v, str);

	if(n != NULL) {
		if(n->var->isFunction()) {
			FuncT* func = n->var->getFunc();
			if(!func->regular) { //class get/set function.
				push(v->ref()); //push this
				if(!funcCall(str, true))
					pop(); //pop and drop this

				return;
			}
		}
	}
	else {
		n = v->addChild(str);
	}

	n->var->ref();	
	push(n);
}

void KoalaJS::runCode(Bytecode* bc) {
	if(bc != NULL) {
		if(code != NULL && bcode != NULL) {
			CodeT cs;
			cs.pc = pc;
			cs.code = code;
			cs.size = codeSize;
			cs.bcode = bcode;
			codeStack.push(cs);	
		}
		
		setBytecode(bc);

		VMScope sc;
		sc.var = root->ref();
		sc.pc = 0;
		pushScope(sc);
	}
	size_t scDeep = scopes.size();
	size_t interuptCount = 0;

	while(pc < codeSize) {
		interuptCount++;
		if(interuptCount >= INTERUPT_COUNT) {
			interuptCount = 0;
			doInterupt();
		}

		PC ins = code[pc++];
		OpCode instr = ins >> 16;
		OpCode offset = ins & 0x0000FFFF;
		string str;

		switch(instr) {
			case INSTR_NIL: {
												break;
											}
			case INSTR_TRUE: {
												 BCVar* v = new BCVar(1);	
												 push(v->ref());
												 break;
											 }
			case INSTR_FALSE: {
													BCVar* v = new BCVar(0);	
													push(v->ref());
													break;
												}
			case INSTR_UNDEF: {
													BCVar* v = new BCVar();	
													push(v->ref());
													break;
												}
			case INSTR_POP: {
												pop();
												break;
											}
			case INSTR_JMP: {
												pc = pc + offset - 1;
												break;
											}
			case INSTR_JMPB: {
												 pc = pc - offset - 1;
												 break;
											 }
			case INSTR_NJMP: {
												 StackItem* i = pop2();
												 if(i != NULL) {
													 BCVar* v = VAR(i);
													 if(v->type == BCVar::UNDEF || v->getInt() == 0)
														 pc = pc + offset - 1;
													 v->unref();
												 }
												 break;
											 }
			case INSTR_NEG: {
												StackItem* i = pop2();
												if(i != NULL) {
													BCVar* v = VAR(i);
													if(v->isInt()) {
														int n = v->getInt();
														v->setInt(-n);
													}
													else if(v->isFloat()) {
														float n = v->getFloat();
														v->setFloat(-n);
													}
													push(v);
												}
												break;
											}
			case INSTR_NOT: {
												StackItem* i = pop2();
												if(i != NULL) {
													BCVar* v = VAR(i);
													int c = 0;
													if(v->type == BCVar::UNDEF || v->getInt() == 0)
														c = 1;
													v->unref();
													v = new BCVar(c);
													push(v->ref());	
												}
												break;
											}
			case INSTR_EQ: 
			case INSTR_NEQ: 
			case INSTR_LES: 
			case INSTR_GRT: 
			case INSTR_LEQ: 
			case INSTR_GEQ: {
												StackItem* i2 = pop2();
												StackItem* i1 = pop2();
												if(i1 != NULL && i2 != NULL) {
													BCVar* v1 = VAR(i1);
													BCVar* v2 = VAR(i2);
													compare(instr, v1, v2);

													v1->unref();
													v2->unref();
												}
												break;
											}
			case INSTR_AAND: 
			case INSTR_OOR: {
												StackItem* i2 = pop2();
												StackItem* i1 = pop2();
												if(i1 != NULL && i2 != NULL) {
													BCVar* v1 = VAR(i1);
													BCVar* v2 = VAR(i2);

													int r = 0;
													if(instr == INSTR_AAND)
														r = (v1->getInt() != 0) && (v2->getInt() != 0);
													else
														r = (v1->getInt() != 0) || (v2->getInt() != 0);
													BCVar* v = new BCVar(r);
													push(v->ref());

													v1->unref();
													v2->unref();
												}
												break;
											}
			case INSTR_PLUS: 
			case INSTR_PLUSEQ: 
			case INSTR_MULTIEQ: 
			case INSTR_DIVEQ: 
			case INSTR_MODEQ: 
			case INSTR_MINUS: 
			case INSTR_MINUSEQ: 
			case INSTR_DIV: 
			case INSTR_MULTI: 
			case INSTR_MOD: {
												StackItem* i2 = pop2();
												StackItem* i1 = pop2();
												if(i1 != NULL && i2 != NULL) {
													BCVar* v1 = VAR(i1);
													BCVar* v2 = VAR(i2);
													mathOp(instr, v1, v2);

													v1->unref();
													v2->unref();
												}
												break;
											}
			case INSTR_MMINUS_PRE: {
															 StackItem* it = pop2();
															 if(it != NULL) {
																 BCVar* v = VAR(it);
																 int i = v->getInt() - 1;
																 v->setInt(i);
																 push(v);
															 }
															 break;
														 }
			case INSTR_MMINUS: {
													 StackItem* it = pop2();
													 if(it != NULL) {
														 BCVar* v = VAR(it);
														 int i = v->getInt();
														 v->setInt(i-1);
														 v->unref();
														 v = new BCVar(i);
														 push(v->ref());
													 }
													 break;
												 }
			case INSTR_PPLUS_PRE: {
															StackItem* it = pop2();
															if(it != NULL) {
																BCVar* v = VAR(it);
																int i = v->getInt() + 1;
																v->setInt(i);
																push(v);
															}
															break;
														}
			case INSTR_PPLUS: {
													StackItem* it = pop2();
													if(it != NULL) {
														BCVar* v = VAR(it);
														int i = v->getInt();
														v->setInt(i+1);
														v->unref();
														v = new BCVar(i);
														push(v->ref());
													}
													break;
												}
			case INSTR_RETURN:  //return without value
			case INSTR_RETURNV: { //return with value
														VMScope* sc = scope();
														if(sc != NULL) {
															BCVar* thisVar = sc->var->getThisVar();
															if(instr == INSTR_RETURN) //return without value, push "this" to stack
																push(thisVar->ref());

															pc = sc->pc;
															if(scDeep == scopes.size() && bc == NULL) { //usually means call js function.
																popScope();
																return;
															}
															popScope();
														}
														break;
													}
			case INSTR_VAR:
			case INSTR_CONST: {
													str = bcode->getStr(offset);
													BCNode *node = find(str);
													if(node != NULL) { //find just in current scope
														if(node->var->isUndefined()) // declared only before
															ERR("Warning: %s has already existed.\n", str.c_str());
													}
													else {
														VMScope* current = scope();
														if(current != NULL) {
															node = current->var->addChild(str);
															if(node != NULL && instr == INSTR_CONST)
																node->beConst = true;
														}
													}
													break;
												}
			case INSTR_LOAD: {
												 str = bcode->getStr(offset);
												 if(str == THIS) {
													 BCVar* v = getCurrentObj(true);
													 if(v != NULL)
														 push(v->ref());
												 }
												 else {
													 BCNode* node = NULL;
													 node = scope()->var->getChild(str);

													 if(node == NULL) {
														 BCVar* thisVar = getCurrentObj();
														 if(thisVar != NULL) {
															 node = thisVar->getChild(str);
															 if(node == NULL)
																 node = findInClass(thisVar, str);
														 }
													 }
													 if(node == NULL) {
														 node = findInScopes(str);
														 if(node == NULL) {
															 ERR("Warning: '%s' undefined!\n", str.c_str());
															 VMScope* current = scope();
															 if(current != NULL) {
																 node = current->var->addChild(str);
															 }
														 }
													 }
													 node->var->ref();
													 push(node);
												 }
												 break;
											 }
			case INSTR_GET: {
												str = bcode->getStr(offset);
												StackItem* i = pop2();
												if(i != NULL) {
													BCVar* v = VAR(i);
													if(v->isString() || v->isObject() || v->isArray()) {
														doGet(v, str);
													}
													v->unref();
												}
												break;
											}
			case INSTR_INT: {
												BCVar* v = new BCVar(BCVar::INT);
												v->setInt((int)code[pc++]);
												push(v->ref());
												break;
											}
			case INSTR_FLOAT: {
													BCVar* v = new BCVar(BCVar::FLOAT);
													v->setFloat(*(float*)(&code[pc++]));
													push(v->ref());
													break;
												}
			case INSTR_STR: {
												BCVar* v = new BCVar(BCVar::STRING);
												v->setString(bcode->getStr(offset));
												push(v->ref());
												break;
											}
			case INSTR_ASIGN: {
													StackItem* i2 = pop2();
													StackItem* i1 = pop2();
													if(i1 != NULL && i1->isNode && i2 != NULL) {
														BCNode* node = (BCNode*)i1;
														BCVar* v = VAR(i2);

														bool modi = (!node->beConst || node->var->type == BCVar::UNDEF);
														node->var->unref();
														if(modi) 
															node->replace(v);
														else
															ERR("Can not change a const variable: %s!\n", node->name.c_str());
														v->unref();
														push(node->var->ref());
													}
													break;
												}
			case INSTR_ARRAY_AT: {
														 StackItem* i2 = pop2();
														 StackItem* i1 = pop2();
														 if(i1 != NULL && i1->isNode && i2 != NULL) {
															 BCNode* node = (BCNode*)i1;

															 BCVar* v = VAR(i2);
															 int at = v->getInt();
															 v->unref();

															 BCNode* n = node->var->getChildOrCreate(at);
															 if(n != NULL) {
																 n->var->ref();
																 push(n);
															 }
															 node->var->unref();
														 }
														 break;
													 }
			case INSTR_OBJ:
			case INSTR_ARRAY: {
													BCVar* obj = new BCVar();
													if(instr == INSTR_OBJ)
														obj->type = BCVar::OBJECT;
													else
														obj->type = BCVar::ARRAY;
													VMScope sc;
													sc.var = obj->ref();
													pushScope(sc);
													break;
												}
			case INSTR_ARRAY_END: 
			case INSTR_OBJ_END: {
														BCVar* obj = scope()->var;
														push(obj->ref()); //that actually means currentObj->ref() for push and unref for unasign.
														popScope();
														break;
													}
			case INSTR_MEMBER: 
			case INSTR_MEMBERN: {
														str = instr == INSTR_MEMBER ? "" : bcode->getStr(offset);
														StackItem* i = pop2();
														if(i != NULL) {
															BCVar* v = VAR(i);
															if(v->isFunction()) {
																FuncT* func = v->getFunc();
																size_t argNum = func->args.size();
																if(argNum > 0)
																	str = str + "$" + StringUtil::from((int)argNum);
															}
															scope()->var->addChild(str, v);
															v->unref();
														}
														break;
													}
			case INSTR_FUNC: 
			case INSTR_FUNC_GET: 
			case INSTR_FUNC_SET: {
														 str = bcode->getStr(offset);
														 BCVar* v = funcDef(str, (instr == INSTR_FUNC ? true:false));
														 if(v != NULL)
															 push(v->ref());
														 break;
													 }
			case INSTR_CLASS: {
													str = bcode->getStr(offset);
													BCVar* v = getOrAddClass(str);
													push(v->ref());
													VMScope sc;
													sc.var = v->ref();
													pushScope(sc);
													break;
												}
			case INSTR_CLASS_END: {
															popScope();
															break;
														}
			case INSTR_CALL: {
												 if(!funcCall(bcode->getStr(offset)))
													 pop();//drop this
												 interuptCount = INTERUPT_COUNT;
												 break;
											 }
			case INSTR_CALLO: {
													if(!funcCall(bcode->getStr(offset), true))
														pop(); //drop this
												 	interuptCount = INTERUPT_COUNT;
													break;
												}
			case INSTR_NEW: {
												doNew(bcode->getStr(offset));
												break;
											}
		}
	}
	popScope();

	if(!codeStack.empty()) {
		CodeT cs = codeStack.top();
		codeStack.pop();

		pc = cs.pc;
		code = cs.code;
		codeSize = cs.size;
		bcode = cs.bcode;
	}
}

