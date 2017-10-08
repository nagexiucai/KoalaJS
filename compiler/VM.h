#ifndef BC_VM_HH
#define BC_VM_HH

#include "Bytecode.h"
#include "Var.h"

#define VAR(i) (i->isNode ? ((BCNode*)i)->var : (BCVar*)i)
class BCVM {
public:
	inline BCVM() {
		pc = 0;
		codeSize = 0;
		code = NULL;
		stackTop = STACK_DEEP;
		root = NULL;
		current = NULL;
		opVar = NULL;
	}

	inline ~BCVM() {
		reset();
	}

	inline void reset() {
		bcode.reset();
		pc = 0;
		codeSize = 0;
		code = NULL;
		current = NULL;

		while(true) {
			StackItem* i = pop();
			if(i == NULL) 
				break;
			BCVar* v = VAR(i);
		}

		if(root != NULL)
			root->unref();
		
		if(opVar != NULL)
			opVar->unref();

		root = NULL;
	}

	void run(const string& fname);

	inline void opv(BCVar* v) {
		if(opVar != NULL) {
			opVar->unref();
		}
		opVar = v->ref();
	}

private:
	PC pc;
	PC* code;
	PC codeSize;
	Bytecode bcode;

	BCVar* root;
	BCVar* current;
	BCVar* opVar;

	void run();

	const static uint16_t STACK_DEEP = 128;
	StackItem* stack[STACK_DEEP];
	uint16_t stackTop;

	void push(StackItem* v);

	StackItem* pop();
};

#endif
