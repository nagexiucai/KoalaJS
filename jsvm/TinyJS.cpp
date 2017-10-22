#include "TinyJS.h"
#include "Compiler.h"
#include "libs/String/StringUtil.h"
#include "libs/File/File.h"
#include <sstream>  
#include <iostream>  

BCVar* CTinyJS::newObject(const string& clsName) {
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

	if(cls->var->type != BCVar::CLASS) {
		ERR("%s is not a class\n", clsName.c_str());
		return NULL;
	}

	ret = new BCVar();
	ret->type = BCVar::OBJECT;
	ret->addChild(PROTOTYPE, cls->var);

	JSCallback nc = cls->var->getNativeConstructor();
	if(nc != NULL)
		nc(ret, NULL);
	return ret;
}

void CTinyJS::run(const std::string &fname) {
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
			compiler.run(cname);
//			compiler.bytecode.dump();
			compiler.bytecode.clone(bc);
			CodeCache::cache(cname, bc);
			runCode(bc);
		}
	}
		
	if(oldCwd.length() > 0)
		cwd = oldCwd;
}

void CTinyJS::exec(const std::string &code) {
	Bytecode* bc = CodeCache::get(code);
	if(bc == NULL) {
		bc = new Bytecode();
		Compiler compiler;
		compiler.exec(code);
		compiler.bytecode.clone(bc);
		CodeCache::cache(code, bc);
	}
	runCode(bc);
}


StackItem* CTinyJS::pop2() {
	if(stackTop == STACK_DEEP) // touch the bottom of stack
		return NULL;

	StackItem* ret =  vStack[stackTop];
	vStack[stackTop] = NULL;
	stackTop++;
	return ret;
}

void CTinyJS::pop() {
	if(stackTop == STACK_DEEP)
		return;

	StackItem* i =  vStack[stackTop];
	vStack[stackTop] = NULL;
	stackTop++;
	VAR(i)->unref();
}

void CTinyJS::push(StackItem* v) {
	if(stackTop == 0) { //stack overflow
		ERR("stack overflow\n");
		return;
	}
	vStack[--stackTop] = v;
}

BCNode* CTinyJS::find(const string& name) {
	VMScope* sc = scope();
	if(sc == NULL)
		return NULL;
		
	return sc->var->getChild(name);
}

BCNode* CTinyJS::findInScopes(const string& name) {
	for(int i=scopes.size() - 1; i >= 0; --i) {
		BCNode* r = scopes[i].var->getChild(name);
		if(r != NULL)
			return r;
	}
	return NULL;
}

BCNode* CTinyJS::findInClass(BCVar* obj, const string& name) {
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

BCVar* CTinyJS::getCurrentObj() {
	return root;
}

void CTinyJS::doNew(const string& clsName) {
	BCVar* ret = newObject(clsName);
	if(ret != NULL)
		push(ret->ref());
}
	
void CTinyJS::funcCall(const string& funcName) {
	BCVar* ret = NULL;

	if(funcName.length() == 0)
		return;
	
	//read object
	StackItem* si = pop2();
	BCVar* object = NULL;
	if(si == NULL)  {
		ERR("Function '%s' not found\n", funcName.c_str());
		return;
	}

	object = VAR(si);	
	//find function in object;
	BCNode*	n = object->getChild(funcName);
	if(n == NULL)
		n = findInClass(object, funcName);

	//find function in scopes;
	if(n == NULL)
		n = findInScopes(funcName);

	if(n == NULL) {
		ERR("Function '%s' not found\n", funcName.c_str());
		object->unref(); //unref after pop
		return;
	}

	if(n->var->type != BCVar::FUNC &&
			n->var->type != BCVar::NFUNC) {
		ERR("%s is not a function\n", funcName.c_str());
		object->unref(); //unref after pop
		return;
	}

	FuncT* func = n->var->getFunc();
	func->thisVar->replace(object);
	object->unref(); //unref after pop

	//read arguments
	for(int i=func->argNum-1; i>=0; --i) {
		BCNode* arg = func->args->getChild(i);
		if(arg == NULL) {
			ERR("%s argument not match\n", funcName.c_str());
			return;
		}

		si = pop2();
		if(si == NULL) {
			ERR("%s argument not match\n", funcName.c_str());
			return;
		}
		BCVar* v = VAR(si);
		arg->replace(v);
		v->unref(); //unref after pop
	}

	if(n->var->type == BCVar::NFUNC) { //native function
		if(func->native != NULL) {
			func->native(n->var, this);
			//read return.
			BCVar* ret = func->returnVar->var;
			push(ret->ref());

			func->resetArgs();
		}
		return;
	}

	//js function
	VMScope sc;
	sc.pc = pc;
	sc.var = n->var;
	scopes.push_back(sc);
	pc = func->pc;
}

BCVar* CTinyJS::funcDef(const string& funcName) {
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

		if(instr == INSTR_FUNC_END) {
			funcPC = pc;
			pc = pc + offset - 1;
			break;
		}
		args.push_back(bcode->getStr(offset));
	}
	//create function variable
	ret = new BCVar();
	int argNum = args.size();
	ret->setFunction(argNum, funcPC);
	//set args as top children 
	for(int i=0; i<argNum; ++i) {
		ret->getFunc()->args->addChild(args[i]);
	}
	//add function to current scope 
	if(funcName.length() > 0) {
		scope()->var->addChild(funcName, ret);
	}
	return ret;
}

BCVar* CTinyJS::addClass(const string& clsName, JSCallback nc) {
		BCNode* cls = root->getChildOrCreate(clsName);
		if(cls == NULL)
			return NULL;

		cls->var->type = BCVar::CLASS;
		if(nc != NULL)
			cls->var->setNativeConstructor(nc);

		return cls->var;
}

void CTinyJS::addNative(const string& clsName, const string& funcDecl, JSCallback native, void* data) {
	BCVar* clsVar = NULL;
	if(clsName.length() == 0) {
		clsVar = root;
	}
	else {
		BCNode* cls = root->getChildOrCreate(clsName);
		if(cls == NULL || native == NULL)
			return;

		cls->var->type = BCVar::CLASS;
		clsVar = cls->var;
	}

	int i = funcDecl.find('(');
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
	int argNum = args.size();
	funcVar->setFunction(argNum, 0, native);
	for(i=0; i<argNum; ++i) {
		funcVar->getFunc()->args->addChild(args[i]);	
	}
	
	clsVar->addChild(funcName, funcVar);
}

void CTinyJS::init() {
	root = new BCVar();
	root->type = BCVar::OBJECT;
	root->ref();
}

void CTinyJS::compare(OpCode op, BCVar* v1, BCVar* v2) {
	float f1, f2;
	f1 = v1->getFloat();
	f2 = v2->getFloat();
	
	bool i = false;
	if(v1->type == v2->type) {
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
	else if(op == INSTR_NEQ) {
		i = true;
	}
	
	BCVar* v = new BCVar(i ? 1 : 0);
	push(v->ref());
}

void CTinyJS::mathOp(OpCode op, BCVar* v1, BCVar* v2) {
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
				ret = (f1 + f2);
				break; 
			case INSTR_MINUS: 
				ret = (f1 - f2);
				break; 
			case INSTR_DIV: 
				ret = (f1 / f2);
				break; 
			case INSTR_MULTI: 
				ret = (f1 * f2);
				break; 
			case INSTR_MOD: 
				ret = (((int)f1) % (int)f2);
				break; 
		}

		BCVar* v;
		if(floatMode)
			v = new BCVar(ret);
		else
			v = new BCVar((int)ret);
		push(v->ref());
		return;
	}

	//do string + 
	if(op == INSTR_PLUS) {
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
		BCVar* v = new BCVar(s);
		push(v->ref());
	}
}

void CTinyJS::doGet(BCVar* v, const string& str) {
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

	BCNode* n = v->getChildOrCreate(str);
	n->var->ref();	
	push(n);
}

void CTinyJS::runCode(Bytecode* bc) {
	if(code != NULL && bcode != NULL) {
		CodeT cs;
		cs.pc = pc;
		cs.code = code;
		cs.size = codeSize;
		cs.bcode = bcode;
		codeStack.push(cs);	
	}

	pc = 0;
	bcode = bc;
	code = bcode->getCode(codeSize);

	VMScope sc;
	BCVar* currentObj = root;
	sc.var = root;
	sc.pc = 0;
	scopes.push_back(sc);


	while(pc < codeSize) {
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
			case INSTR_PLUS: 
			case INSTR_MINUS: 
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
					FuncT* func = sc->var->getFunc();
					if(func != NULL)
						func->resetArgs();

					pc = sc->pc;
					scopes.pop_back();
				}
				break;
			}
			case INSTR_VAR:
			case INSTR_CONST: {
				str = bcode->getStr(offset);
				BCNode *node = find(str);
				if(node != NULL) { //find just in current scope
					if(node->var->isUndefined()) // declared only before
						ERR("%s has already existed.\n", str.c_str());
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
					BCVar* v = getCurrentObj();
					if(v != NULL)
						push(v->ref());
				}
				else {
					BCNode* node = findInScopes(str);
					if(node == NULL) {
						VMScope* current = scope();
						if(current != NULL) {
							node = current->var->addChild(str);
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
					if(modi) node->replace(v);
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
			case INSTR_OBJ: {
				BCVar* obj = new BCVar();
				obj->type = BCVar::OBJECT;
				push(currentObj->ref());
				currentObj = obj->ref(); //need unref for unasign
				break;
			}
			case INSTR_OBJ_END: {
				StackItem* i = pop2();
				BCVar* obj = (BCVar*)i;
				push(currentObj); //that actually means currentObj->ref() for push and unref for unasign.
				currentObj = obj;
				obj->unref();
				break;
			}
			case INSTR_MEMBER: {
				StackItem* i2 = pop2();
				StackItem* i1 = pop2();
				if(i1 != NULL && i2 != NULL) {
					BCVar* nv = VAR(i1);
					BCVar* v = VAR(i2);
					string name = nv->getString();
					nv->unref();

					currentObj->addChild(name, v);
					v->unref();
				}
				break;
			}
			case INSTR_FUNC: {
				BCVar* v = funcDef(bcode->getStr(offset));
				if(v != NULL)
					push(v->ref());
				break;
			}
			case INSTR_CALL: {
				funcCall(bcode->getStr(offset));
				break;
			}
			case INSTR_NEW: {
				doNew(bcode->getStr(offset));
				break;
			}
		}
	}
	scopes.pop_back();

	if(!codeStack.empty()) {
		CodeT cs = codeStack.top();
		codeStack.pop();

		pc = cs.pc;
		code = cs.code;
		codeSize = cs.size;
		bcode = cs.bcode;
	}
}

