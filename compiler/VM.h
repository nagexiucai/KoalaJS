#ifndef BC_VM_HH
#define BC_VM_HH

#include "Bytecode.h"
#include "Var.h"

#define VAR(i) (i->isNode ? ((BCNode*)i)->var : (BCVar*)i)
#define ERR(e) throw new CScriptException(e)
class VM {
public:
	inline VM() {
		pc = 0;
		codeSize = 0;
		code = NULL;
		stackTop = STACK_DEEP;
		root = NULL;
	}

	inline ~VM() {
		reset();
	}

	inline void reset() {
		bcode.reset();
		pc = 0;
		codeSize = 0;
		code = NULL;

		while(true) {
			StackItem* i = pop2();
			if(i == NULL) 
				break;
			BCVar* v = VAR(i);
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
	vector<BCVar*> scopes;

	void run();

	const static uint16_t STACK_DEEP = 128;
	StackItem* stack[STACK_DEEP];
	uint16_t stackTop;

	void push(StackItem* v);

	StackItem* pop2();

	/**find node by name just in current scope
	@param name, name of variable;
	*/
	BCNode* find(const string& name);

	/**find node by name in all scopes
	@param name, name of variable;
	*/
	BCNode* findInScopes(const string& name);

	inline BCVar* scope() { 
		int i = scopes.size() - 1;
		return (i < 0 ? NULL : scopes[i]);
	}

	//pop stack and release it.
	void pop();

	BCVar* funcDef(const string& funcName);

	BCVar* funcCall(const string& funcName);
};

#endif
