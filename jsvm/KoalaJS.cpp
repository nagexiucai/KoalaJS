#include "KoalaJS.h"
#include "Compiler.h"
#include "utils/String/StringUtil.h"
#include "utils/File/File.h"
#include "utils/Thread/Thread.h"
#include <sstream>  
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define DEBUG_LINE bcode->getDebugLine(pc-1).c_str()

static GlobalVars _globalVars;

#define LOADER "_moduleLoader"
bool KoalaJS::loadModule(const string& fname) {
	std::map<string, void*>::iterator it = extDL.find(fname);
	void* h = NULL;
	if(it == extDL.end()) {
		h = dlopen(fname.c_str(), RTLD_LAZY);
		if(h == NULL) {
			return false;
		}
		extDL.insert(pair<string, void*>(fname, h));
		it = extDL.find(fname);
	}
	else {
		h = it->second;
	}
	if(h == NULL)
		return false;

	JSModuleLoader loader = (JSModuleLoader)dlsym(h, LOADER);
	if(loader == NULL) {
		dlclose(h);
		extDL.erase (it);
		ERR("Extended module load-function not defined (%s)! %s\n", fname.c_str(), DEBUG_LINE);
		return false;
	}

	loader(this);
	return true;
}

void KoalaJS::loadModules() {
	std::map<string, void*>::iterator it;
	for(it = extDL.begin(); it != extDL.end(); ++it) {
		void *h = it->second;
		if(h == NULL)
			continue;
		JSModuleLoader loader = (JSModuleLoader)dlsym(h, LOADER);
		if(loader != NULL) {
			loader(this);
		}
	}
}

GlobalVars* KoalaJS::getGlobalVars() {
	return &_globalVars;
}

void KoalaJS::doInterrupt() {
	if(interrupter != NULL)
		interrupter->doInterrupt();
}


BCVar* KoalaJS::runJSFunc(BCVar* obj, BCVar* funcV, const vector<BCVar*>* args) {
	VMScope sc;
	sc.var = obj->ref();
	sc.pc = pc;
	pushScope(sc);

	size_t scDeep = scopes.size();
	funcCall(obj, funcV, args);
	if(scDeep < scopes.size()) {
		runCode(NULL);
	}
	popScope();

	StackItem* i = pop2();
	BCVar* v = NULL;
	if(i != NULL)
		v = VAR(i);
	else {
		v = new BCVar();
		v->ref();
	}
	return v;
}

BCVar* KoalaJS::callJSFunc(const string& funcName, const vector<BCVar*>& args, BCVar* obj) {
	BCVar* v = NULL;

	size_t argNum = args.size();
	string fname = funcName;
	if(argNum > 0) {
		fname = fname + "$" + StringUtil::from((int)argNum);
	}

	if(obj == NULL)
		obj = root;

	BCNode *func = findFunc(obj, fname, false);
	if(func == NULL) {
		v = new BCVar();
		v->ref();
		return v;
	}
	return runJSFunc(obj, func->var, &args);
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
		ERR("Class %s not found!%s\n", clsName.c_str(), DEBUG_LINE);
		return NULL;
	}

	return newObject(cls);
}

BCVar* KoalaJS::newObject(BCNode* cls) {
	BCVar* ret = NULL;
	if(cls->var->type != BCVar::CLASS) {
		ERR("%s is not a class!%s\n", cls->name.c_str(), DEBUG_LINE);
		return NULL;
	}

	ret = new BCVar();
	ret->type = BCVar::OBJECT;
	ret->addChild(PROTOTYPE, cls->var);
	return ret;
}

bool KoalaJS::run(const string& fname, bool debug, bool repeat) {
	bool res = true;
#ifdef KOALA_DEBUG
	debugMode = debug;
#endif
	Bytecode bc;

	if(bcode == NULL)
		bcode = &bc;
	PC start = bcode->endPC();

	std::string oldCwd = cwd;
	cname = File::getFullname(cwd, fname);
	cwd = File::getPath(cname);

	if(fname.find(".bcode") != string::npos) {
      bcode->fromFile(cname);
			runCode(bcode, 0);
	}
	else {
		std::map<string, PC>::iterator it = extJSC.find(cname);
		if(it == extJSC.end()) { 
			Compiler compiler;
			if(!compiler.run(cname, bcode, debug)) 
				res = false;

			runCode(bcode, start);
			extJSC.insert(pair<string, PC>(cname, start));
		}
		else if(repeat) {
			start = it->second;
			runCode(bcode, start);
		}
	}

	if(oldCwd.length() > 0)
		cwd = oldCwd;
	return res;
}

bool KoalaJS::exec(const std::string &code, bool repeat) {
	bool res = true;
	PC start = bcode->endPC();

	std::map<string, PC>::iterator it = extJSC.find(code);
	if(it == extJSC.end()) { 
		Compiler compiler;
		if(!compiler.exec(code, bcode))
			res = false;

		runCode(bcode, start);
		extJSC.insert(pair<string, PC>(code, start));
	}
	else if(repeat) {
		start = it->second;
		runCode(bcode, start);
	}
	return res;
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

	/*while(obj != NULL) {
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
	*/

	n = obj->getChild(PROTOTYPE);
	if(n == NULL)
		return NULL;
	obj = n->var;

	while(obj != NULL) {
		n = obj->getChild(name);
		if(n != NULL) 
			return n;

		n = obj->getChild(SUPER);
		if(n == NULL)
			return NULL;

		obj = n->var;
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
	string fname = CONSTRUCTOR;
	if(argNum > 0) {
		fname = fname + "$" + StringUtil::from(argNum);
	}

	vector<BCVar*> args;
	prepareFuncArgs(argNum, args);

	BCNode* func;
	//do super constructor
	vector<BCVar*> supers;
	BCVar* v = obj;
	while(true) {
		v = v->getSuperVar();
		if(v == NULL)
			break;
		supers.push_back(v);
	}

	size_t sz = supers.size();
	for(size_t i=sz; i>0; --i) {
		v = supers[i-1];
		if(v != NULL) {
			func = findFunc(v, fname, true);
			if(func != NULL) {
				BCVar* r = runJSFunc(obj, func->var, &args);
				if(r != NULL)
					r->unref();
			}
		}
	}
	//do constructor
	func = findFunc(obj, fname, true);
	if(func != NULL) {
		BCVar* r = runJSFunc(obj, func->var, &args);
		if(r != NULL)
			r->unref();
	}
	releaseFuncArgs(args);
	return true;
}

void KoalaJS::doNew(const string& clsName) { 
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
		if(argNum == 1)
			return;

		ret = new BCVar();
		ret->type = BCVar::OBJECT;
	}
	else {
		BCNode* cls = findInScopes(clsName);
		if(cls == NULL && pos != string::npos)
			cls = findInScopes(cn);

		if(cls == NULL) {
			ERR("Class %s not found!%s\n", cn.c_str(), DEBUG_LINE);
			return;
		}
		if(cls->var->isFunction()) {
			BCVar* v = getCurrentObj(true);
			funcCall(v, cls->var);
			return;
		}
		ret = newObject(cls);
		if(ret == NULL)
			return;
		ret->ref();
		construct(ret, argNum);
		push(ret);
		return;
	}

	if(ret != NULL)
		push(ret->ref());
}

void KoalaJS::doExtends(BCVar* v, const string& clsName) { 
	BCNode* cls = findInScopes(clsName);
	if(cls == NULL) {
		ERR("Super Class %s not found!%s\n", clsName.c_str(), DEBUG_LINE);
		return;
	}
	v->addChild(SUPER, cls->var);
}

BCNode* KoalaJS::findMemberFunc(BCVar* owner, const string& fname) {
	//find function in object;
	BCNode*	n = owner->getChild(fname);
	if(n == NULL) {
		n = findInClass(owner, fname);
	}

	if(n == NULL || !n->var->isFunction()) {
		return NULL;
	}
	return n;
}

BCNode* KoalaJS::findFunc(BCVar* owner, const string& fname, bool member) {
	//find function in object;
	BCNode*	n = findMemberFunc(owner, fname);
	size_t pos = fname.find("$");
	if(n == NULL && pos != string::npos) {
		string fn = fname.substr(0, pos);
		n = findMemberFunc(owner, fn);
		if(n != NULL) {
			string args = fname.substr(pos+1);
			int argsNum = atoi(args.c_str());
			if(argsNum != (int)n->var->getFunc()->args.size())
				n = NULL;
		}
	}
	//find function in scopes;
	if(n == NULL)
		n = findInScopes(fname);

	if(n == NULL || !n->var->isFunction()) {
		if(fname != CONSTRUCTOR)
			ERR("Can not find function '%s'! %s\n", fname.c_str(), DEBUG_LINE);
		return NULL;
	}
	return n;
}

bool KoalaJS::prepareFuncArgs(int argNum, vector<BCVar*>& args) {
//read arguments
	for(int i=0; i<argNum; ++i) {
		StackItem* si = pop2();
		if(si == NULL) {
			return false;
		}
		BCVar* v = VAR(si);
		args.insert(args.begin(), v);
	}
	return true;
}

void KoalaJS::releaseFuncArgs(vector<BCVar*>& args) {
//read arguments
	int argNum = (int)args.size();
	for(int i=0; i<argNum; ++i) {
		BCVar* v = args[i];
		v->unref();
	}
	args.clear();
}

bool KoalaJS::funcCall(BCVar* object, BCVar* funcV, const vector<BCVar*>* args) {
	BCVar* params = new BCVar(); //function parameters
	params->type = BCVar::OBJECT;
	FuncT* func = funcV->getFunc();
	if(object != NULL)
		params->addChild(THIS, object);

	//read arguments
	int argNum = (int)func->args.size();
	if(args == NULL) { //read arguments from stack
		for(int i=argNum-1; i>= 0; --i) {
			string arg = func->args[i];
			StackItem* si = pop2();
			if(si == NULL) {
				delete params;
				return false;
			}
			BCVar* v = VAR(si);
			params->addChild(arg, v);
			v->unref(); //unref after pop
		}

		if(object == NULL) {
			StackItem* si = pop2();
			object = VAR(si);
			params->addChild(THIS, object);
			object->unref();
		}
	}
	else {
		for(int i=argNum-1; i>= 0; --i) {
			BCVar* v = (*args)[i];
			string arg = func->args[i];
			params->addChild(arg, v);
		}
	}

	VMScope sc;
	sc.pc = pc;
	sc.var = params->ref();
	pushScope(sc);

	if(funcV->type == BCVar::NFUNC) { //native function
		if(func->native != NULL) {
			func->native(this, params, func->data);
			//read return.
			BCVar* ret = params->getReturnVar();
			push(ret->ref());
		}
		popScope();
		return true; 
	}

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
			ERR("Function '%s' has already been defined! %s\n", funcName.c_str(), DEBUG_LINE);			
			return NULL;
		}
	}
	//read arguments
	PC funcPC = 0;
	while(true) {
		PC ins = code[pc++];
		OprCode instr = ins >> 16;
		OprCode offset = ins & 0x0000FFFF;

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

BCVar* KoalaJS::loadClass(const string& clsName, const string& jsFile) {
	BCNode* cls = root->getChild(clsName);
	if(cls == NULL) {
		if(!run(jsFile, false))
			return NULL;
		cls = root->getChild(clsName);
		if(cls == NULL)
			return NULL;
	}
	return cls->var;
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

		BCVar* inter = new BCVar();
		inter->type = BCVar::OBJECT;
		root->addChild(INTERRUPTER, inter);
	}
	else
		root = rt;

	root->ref();
}

void KoalaJS::compare(OprCode op, BCVar* v1, BCVar* v2) {
	float f1, f2;

	f1 = v1->getFloat();
	f2 = v2->getFloat();

	bool i = false;
	if(v1 == v2) {
		i = true;
	}
	else if(v1->type == v2->type) {
		if(v1->isUndefined()) {
			switch(op) {
				case INSTR_EQ: 
				case INSTR_TEQ:
					i = v2->isUndefined();
					break; 
				case INSTR_NEQ: 
				case INSTR_NTEQ:
					i = !v2->isUndefined();
					break;
			}
		}
		else if(v1->isString()) {
			switch(op) {
				case INSTR_EQ: 
				case INSTR_TEQ:
					i = (v1->getString() == v2->getString());
					break; 
				case INSTR_NEQ: 
				case INSTR_NTEQ:
					i = (v1->getString() != v2->getString());
					break;
			}
		}
		else if(v1->isInt() || v1->isFloat()) {
			switch(op) {
				case INSTR_EQ: 
				case INSTR_TEQ:
					i = (f1 == f2);
					break; 
				case INSTR_NEQ: 
				case INSTR_NTEQ:
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
	else if(op == INSTR_NEQ || op == INSTR_NTEQ) {
		i = true;
	}

	BCVar* v = new BCVar(i ? 1 : 0);
	push(v->ref());
}

void KoalaJS::mathOp(OprCode op, BCVar* v1, BCVar* v2) {
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
			case INSTR_RSHIFT: 
				ret = (((int)f1) >> (int)f2);
				break; 
			case INSTR_LSHIFT: 
				ret = (((int)f1) << (int)f2);
				break; 
			case INSTR_AND: 
				ret = (((int)f1) & (int)f2);
				break; 
			case INSTR_OR: 
				ret = (((int)f1) | (int)f2);
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
				funcCall(v, n->var);
				return;
			}
		}
	}
	else {
		if(!v->isObject())
			ERR("Can not get member %s! %s\n", str.c_str(), DEBUG_LINE);

		if(v->isUndefined()) 
			v->type = BCVar::OBJECT;
		n = v->addChild(str);
	}

	n->var->ref();	
	push(n);
}

BCNode* KoalaJS::load(const string& name, bool create) {
	BCNode* node = NULL;
	VMScope* current = scope();
	node = current->var->getChild(name);

	if(node == NULL) {
		BCVar* thisVar = getCurrentObj();
		if(thisVar != NULL) {
			node = thisVar->getChild(name);
			if(node == NULL)
				node = findInClass(thisVar, name);
		}
	}
	if(node == NULL) {
		node = findInScopes(name);
		if(node == NULL) {
			if(!create)
				return NULL;

			if(name != THIS)
				ERR("Warning: '%s' undefined! %s\n", name.c_str(), DEBUG_LINE);
			node = current->var->addChild(name);
		}
	}
	return node;
}

void KoalaJS::runCode(Bytecode* bc, PC startPC) {
	size_t scDeep = scopes.size();
	bool needPopSC = false;
	if(bc != NULL) {
		setBytecode(bc);

		if(scDeep == 0) {
			VMScope sc;
			sc.var = root->ref();
			sc.pc = pc;
			pushScope(sc);
			needPopSC = true;
			scDeep++;
		}
		pc = startPC;
	}

	while(pc < codeSize) {
#ifdef KOALA_DEBUG
		if(bcode->isDebug()) {
			debug.debug(this, pc);
		}
#endif

		PC ins = code[pc++];
		OprCode instr = ins >> 16;
		OprCode offset = ins & 0x0000FFFF;
		string str;

		if(instr == INSTR_END)
			break;
		
		switch(instr) {
			case INSTR_NIL: {
												break;
											}
			case INSTR_BLOCK: {
													VMScope bl;
													bl.var = scope()->var->ref();
													pushScope(bl);
													break;
												}
			case INSTR_BLOCK_END: {
															popScope();
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
			case INSTR_TEQ:
			case INSTR_NTEQ:
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
			case INSTR_RSHIFT: 
			case INSTR_LSHIFT: 
			case INSTR_AND: 
			case INSTR_OR: 
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
															if(instr == INSTR_RETURN) {//return without value, push "this" to stack
																BCVar* thisVar = sc->var->getThisVar();
																push(thisVar->ref());
															}
															else {
																StackItem* it = pop2();
																if(it != NULL)
																push(VAR(it));
															}

															pc = sc->pc;
															if(scDeep == scopes.size() && bc == NULL) { //usually means call js function.
																popScope();
																return;
															}
															popScope();
															if(scopes.size() == 0)  //scope size = 0, not return in function,means exit.
																return;
														}
														break;
													}
			case INSTR_VAR:
			case INSTR_CONST: {
													str = bcode->getStr(offset);
													BCNode *node = find(str);
													if(node != NULL) { //find just in current scope
														if(node->var->isUndefined()) // declared only before
															ERR("Warning: %s has already existed. %s\n", str.c_str(), DEBUG_LINE);
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
												 BCNode* node = load(str, true); //load variable, create if not exist.
												 node->var->ref();
												 push(node);
												 break;
											 }
			case INSTR_GET: {
												str = bcode->getStr(offset);
												StackItem* i = pop2();
												if(i != NULL) {
													BCVar* v = VAR(i);
													doGet(v, str);
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
															ERR("Can not change a const variable: %s! %s\n", node->name.c_str(), DEBUG_LINE);
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
													//read extends
													ins = code[pc];
													instr = ins >> 16;
													if(instr == INSTR_EXTENDS) {
														pc++;
														offset = ins & 0x0000FFFF;
														str = bcode->getStr(offset);
														doExtends(v, str);
													}

													VMScope sc;
													sc.var = v->ref();
													pushScope(sc);
													break;
												}
			case INSTR_CLASS_END: {
															BCVar* v = scope()->var;
															push(v->ref());
															popScope();
															break;
														}
			case INSTR_CALL: 
			case INSTR_CALLO: {
												 str = bcode->getStr(offset);
												 size_t pos = str.find("$");
												 int argsNum = 0;
												 if(pos != string::npos) {
													 string args = str.substr(pos+1);
													 argsNum = atoi(args.c_str());
												 }

												 StackItem* i = vStack[stackTop+argsNum];
												 BCVar* obj = VAR(i);
												 BCNode* func = findFunc(obj, str, true);
												 if(func != NULL) {
													 funcCall(NULL, func->var);
												 }
												 doInterrupt();
												 break;
												}
			case INSTR_NEW: {
												doNew(bcode->getStr(offset));
												break;
											}
			case INSTR_TYPEOF: {
												StackItem* i = pop2();
												BCVar* var = VAR(i);
												string tp = var ? var->getTypeString() : "null" ;
												BCVar *type = new BCVar(tp);
												push(type->ref());
												break;
											}
			case INSTR_THROW: {
				BCVar *var = reinterpret_cast<BCVar*>(pop2());
				exception = var->ref();

				// walk through scopes to find exception handle
				VMScope* sc = scope();
				while(sc != NULL) {
					PC target = bcode->getTryTarget(pc);
					if(target != ILLEGAL_PC) {
						pc = target;
						break;
					}
					pc = sc->pc;
					popScope();
					sc = scope();
				}
				if(sc == NULL) {
					ERR("uncaught exception:%s\n", exception->getString().c_str());
					return;
				}

				break;
			}
			case INSTR_MOV_EXCP: {
				BCNode *node = reinterpret_cast<BCNode*>(pop2());
				node->replace(VAR(exception));
				exception = NULL;
				break;
			}
		}
	}
	if(needPopSC)
		popScope();
}

