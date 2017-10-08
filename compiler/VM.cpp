#include "VM.h"

void VM::run(const string& fname) {
	reset();

	if(!bcode.fromFile(fname))
		return;

	bcode.dump();
	
	code = bcode.getCode(codeSize);
	run();
}

StackItem* VM::pop2() {
	if(stackTop == STACK_DEEP) // touch the bottom of stack
		return NULL;

	return stack[stackTop++];
}

void VM::pop() {
	if(stackTop == STACK_DEEP)
		return;
	
	StackItem* i = stack[stackTop++];
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
	BCVar* sc = scope();
	if(sc == NULL)
		return NULL;
		
	return sc->getChild(name);
}

BCNode* VM::findInScopes(const string& name) {
	for(int i=scopes.size() - 1; i >= 0; --i) {
		BCVar* scope = scopes[i];
		BCNode* r = scope->getChild(name);
		if(r != NULL)
			return r;
	}
	return NULL;
}

BCVar* VM::funcCall(const string& funcName) {
	BCVar* ret = NULL;

	if(funcName.length() == 0)
		return NULL;
		
	BCNode* n = findInScopes(funcName);
	if(n == NULL) {
			ERR(funcName + " function not defined");			
			return NULL;
	}

	if(n->var->type != BCVar::FUNC &&
			n->var->type != BCVar::NFUNC) {
		ERR(funcName + " is not a function");			
		return NULL;
	}

	FuncT* func = n->var->getFunc();
	//read arguments
	for(int i=func->argNum-1; i>=0; --i) {
			BCNode* arg = n->var->getChild(i);
			if(arg == NULL) {
				ERR(funcName + " argment not match");
				return NULL;
			}

			StackItem* si = pop2();
			if(si == NULL) {
				ERR(funcName + " argment(s) missed");
				return NULL;
			}
			BCVar* v = VAR(si);
			arg->replace(v);
			v->unref(); //unref after pop
	}

	scopes.push_back(n->var);
	pc = func->pc;
	return ret;
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
	while(true) {
		PC ins = code[pc++];
		OpCode instr = ins >> 16;
		OpCode offset = ins & 0x0000FFFF;

		if(instr == INSTR_FUNC_END) {
			pc = pc + offset - 1;
			break;
		}
		args.push_back(bcode.getStr(offset));
	}
	//create function variable
	ret = new BCVar();
	int argNum = args.size();
	ret->setFunction(argNum, pc);
	//set args as top children 
	for(int i=0; i<argNum; ++i) {
		ret->addChild(args[i]);
	}
	//add function to current scope 
	if(funcName.length() > 0) {
		scope()->addChild(funcName, ret);
	}
	return ret;
}

void VM::run() {
	root = new BCVar();
	root->ref();
	scopes.push_back(root);

	while(pc < codeSize) {
		PC ins = code[pc++];
		OpCode instr = ins >> 16;
		OpCode offset = ins & 0x0000FFFF;
		string str;

		switch(instr) {
			case INSTR_NIL: {
				pop();
				break;
			}
			case INSTR_VAR:
			case INSTR_CONST: {
				pop();
				str = bcode.getStr(offset);
				if(find(str)) { //find just in current scope
					ERR((str + " has already existed").c_str());
				}
				else {
					BCVar* current = scope();
					BCNode* node = current->addChild(str);
					if(node != NULL && instr == INSTR_CONST)
						node->beConst = true;
				}
				break;
			}
			case INSTR_LOAD: {
				str = bcode.getStr(offset);
				BCNode* node = findInScopes(str);
				if(node == NULL) {
					BCVar* current = scope();
					node = current->addChild(str);
				}
				node->var->ref();
				push(node);
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
				if(offset == 0xFFFF)
					pop();

				BCVar* v = funcDef(bcode.getStr(offset));
				if(v != NULL)
					push(v->ref());
				break;
			}
			case INSTR_CALL: {
				BCVar* v = funcCall(bcode.getStr(offset));
				if(v != NULL)
					push(v->ref());
				break;
			}
		}
	}
	pop();
	scopes.pop_back();
}

