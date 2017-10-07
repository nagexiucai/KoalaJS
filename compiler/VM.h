#ifndef BC_VM_HH
#define BC_VM_HH

#include "Bytecode.h"
#include "Var.h"

class BCVM {
public:
	inline BCVM() {
		pc = 0;
		codeSize = 0;
		code = NULL;
		stackTop = STACK_DEEP;
		root = NULL;
		current = NULL;
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
			BCVar* v = pop();
			if(v == NULL) 
				break;
			v->unref();
		}

		if(root != NULL)
			root->unref();

		root = NULL;
	}

	void run(const string& fname);

private:
	PC pc;
	PC* code;
	PC codeSize;
	Bytecode bcode;

	BCVar* root;
	BCVar* current;

	void run();

	const static uint16_t STACK_DEEP = 128;
	BCVar* stack[STACK_DEEP];
	uint16_t stackTop;

	void push(BCVar* v);

	BCVar* pop();
};

#endif
