#include "VM.h"

void VM::run(const string& fname) {
	if(!bcode.fromFile(fname))
		return;

	bcode.dump();
	
	code = bcode.getCode(codeSize);
	run();
}

StackItem* VM::pop2() {
	if(stackTop == STACK_DEEP) // touch the bottom of stack
		return NULL;

	StackItem* ret =  stack[stackTop];
	stack[stackTop] = NULL;
	stackTop++;
	return ret;
}

void VM::pop() {
	if(stackTop == STACK_DEEP)
		return;

	StackItem* i =  stack[stackTop];
	stack[stackTop] = NULL;
	stackTop++;
	VAR(i)->unref();
}

void VM::push(StackItem* v) {
	if(stackTop == 0) { //stack overflow
		ERR("stack overflow");
		return;
	}
	stack[--stackTop] = v;
}

BCNode* VM::find(const string& name) {
	VMScope* sc = scope();
	if(sc == NULL)
		return NULL;
		
	return sc->var->getChild(name);
}

BCNode* VM::findInScopes(const string& name) {
	for(int i=scopes.size() - 1; i >= 0; --i) {
		BCNode* r = scopes[i].var->getChild(name);
		if(r != NULL)
			return r;
	}
	return NULL;
}

BCNode* VM::findInClass(BCVar* obj, const string& name) {
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

BCVar* VM::getCurrentObj() {
	return root;
}

void VM::funcCall(const string& funcName) {
	BCVar* ret = NULL;

	if(funcName.length() == 0)
		return;
	
	//read object
	StackItem* si = pop2();
	BCVar* object = NULL;
	if(si == NULL)  {
		ERR(funcName + " function owner(object) not found");			
		return;
	}

	object = VAR(si);	
	//find function in scopes;
	BCNode* n = findInScopes(funcName);
	if(n == NULL) {
			//find function in object;
			n = object->getChild(funcName);
			if(n == NULL)
				n = findInClass(object, funcName);
	}

	if(n == NULL) {
		ERR(funcName + " function not defined");			
		object->unref(); //unref after pop
		return;
	}

	if(n->var->type != BCVar::FUNC &&
			n->var->type != BCVar::NFUNC) {
		ERR(funcName + " is not a function");			
		object->unref(); //unref after pop
		return;
	}

	BCNode* arg = NULL;
	arg = n->var->getChildOrCreate(THIS);
	arg->replace(object);
	object->unref(); //unref after pop

	FuncT* func = n->var->getFunc();
	//read arguments
	for(int i=func->argNum-1; i>=0; --i) {
		arg = n->var->getChild(i);
		if(arg == NULL) {
			ERR(funcName + " argment not match");
			return;
		}

		si = pop2();
		if(si == NULL) {
			ERR(funcName + " argment(s) missed");
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
			BCNode* rn = n->var->getChild(RETURN);
			BCVar* ret;
			if(rn== NULL)
				ret = new BCVar();
			else
				ret = rn->var;
			push(ret->ref());
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

BCVar* VM::funcDef(const string& funcName) {
	BCVar* ret = NULL;
	vector<string> args;

	//check redefined.
	if(funcName.length() > 0) {
		BCNode* n = findInScopes(funcName);
		if(n != NULL) {
			ERR(funcName + " function has already been defined");			
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
		args.push_back(bcode.getStr(offset));
	}
	//create function variable
	ret = new BCVar();
	int argNum = args.size();
	ret->setFunction(argNum, funcPC);
	//set args as top children 
	for(int i=0; i<argNum; ++i) {
		ret->addChild(args[i]);
	}
	//add function to current scope 
	if(funcName.length() > 0) {
		scope()->var->addChild(funcName, ret);
	}
	return ret;
}

void VM::registerNative(const string& clsName, const string& funcDecl, JSCallback native) {
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
		ERR("Register native function '(' missed");	
		return;
	}
	//read func name	
	string funcName = funcDecl.substr(0, i);
	if(funcName.length() == 0) {	
		ERR("Register native function name missed");	
		return;
	}

	//read func args
	string s = funcDecl.substr(i+1);
	i = s.rfind(')');
	if(i == string::npos) {
		ERR("Register native function ')' missed");	
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

		if(arg.length() == 0) 
			break;
		args.push_back(arg);
	}

	BCVar* funcVar = new BCVar();
	int argNum = args.size();
	funcVar->setFunction(argNum, 0, native);
	for(i=0; i<argNum; ++i) {
		funcVar->addChild(args[i]);	
	}
	
	clsVar->addChild(funcName, funcVar);
}

void VM::init() {
	root = VM::newObject("")->ref();
}

void VM::run() {
	VMScope sc;
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
			case INSTR_POP: {
				pop();
				break;
			}
			case INSTR_RETURN: {
				BCVar* v = new BCVar();
				push(v->ref());
			}
			case INSTR_RETURNV: {
				VMScope* sc = scope();
				if(sc != NULL) {
					pc = sc->pc;
					scopes.pop_back();
				}
				break;
			}
			case INSTR_VAR:
			case INSTR_CONST: {
				str = bcode.getStr(offset);
				if(find(str)) { //find just in current scope
					ERR((str + " has already existed").c_str());
				}
				else {
					VMScope* current = scope();
					if(current != NULL) {
						BCNode* node = current->var->addChild(str);
						if(node != NULL && instr == INSTR_CONST)
							node->beConst = true;
					}
				}
				break;
			}
			case INSTR_LOAD: {
				str = bcode.getStr(offset);
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
				v->setString(bcode.getStr(offset));
				push(v->ref());
				break;
			}
			case INSTR_ASIGN: {
				StackItem* i1 = pop2();
				StackItem* i2 = pop2();
				if(i2 != NULL && i2->isNode && i1 != NULL) {
					BCNode* node = (BCNode*)i2;
					BCVar* v = VAR(i1);

					node->var->unref();
					node->replace(v);
					v->unref();
					push(node->var->ref());
				}
				break;
			}
			case INSTR_FUNC: {
				BCVar* v = funcDef(bcode.getStr(offset));
				if(v != NULL)
					push(v->ref());
				break;
			}
			case INSTR_CALL: {
				funcCall(bcode.getStr(offset));
				break;
			}
		}
	}
	scopes.pop_back();
}

