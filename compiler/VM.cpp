#include "VM.h"

void BCVM::run(const string& fname) {
	reset();

	if(!bcode.fromFile(fname))
		return;

	bcode.dump();
	
	code = bcode.getCode(codeSize);
	run();
}

StackItem* BCVM::pop() {
	if(stackTop == STACK_DEEP) // touch the bottom of stack
		return NULL;

	return stack[stackTop++];
}

void BCVM::push(StackItem* v) {
	if(stackTop == 0) { //stack overflow
		throw new CScriptException("stack overflow");
		return;
	}
	stack[--stackTop] = v;
}

void BCVM::run() {
	root = new BCVar();
	current  = root->ref();

	while(pc < codeSize && current != NULL) {
		PC ins = code[pc++];
		Instr instr = ins >> 16;
		Instr offset = ins & 0x0000FFFF;
		string str;

		switch(instr) {
			case INSTR_VAR:
			case INSTR_CONST: {
				str = bcode.getStr(offset);
				if(current->getChild(str)) {
					throw new CScriptException((str + " has already existed").c_str());
				}
				else {
					BCNode* node = current->addChild(str);
					if(node != NULL && instr == INSTR_CONST)
						node->beConst = true;
				}
				break;
			}
			case INSTR_LOAD: {
				BCNode* node = current->getChild(bcode.getStr(offset), true);
				if(node == NULL) 
					throw new CScriptException((str + " not found").c_str());
				else {
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
				StackItem* i1 = pop();
				StackItem* i2 = pop();
				if(i2 != NULL && i2->isNode && i1 != NULL) {
					BCNode* node = (BCNode*)i2;
					BCVar* v = VAR(i1);

					node->var->unref();
					node->replace(v);
					v->unref();
					opv(node->var);
				}
				break;
			}
		}
	}
}

