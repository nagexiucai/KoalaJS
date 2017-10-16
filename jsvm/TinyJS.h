#ifndef BC_CTinyJS_HH
#define BC_CTinyJS_HH

#include "Bytecode.h"
#include "Var.h"

#define VAR(i) (i->isNode ? ((BCNode*)i)->var : (BCVar*)i)

#define ERR(...) fprintf (stderr, __VA_ARGS__)

typedef struct {
	BCVar* var;
	PC pc; //stack pc
} VMScope;


class CTinyJS;
typedef void (*JSModuleLoader)(CTinyJS *tinyJS);

class CTinyJS {
public:
	inline CTinyJS() {
		moduleLoader = NULL;
		pc = 0;
		codeSize = 0;
		code = NULL;
		stackTop = STACK_DEEP;
		root = NULL;
		init();
	}

	inline ~CTinyJS() {
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
			v->unref();
		}

		if(root != NULL) {
			delete root;
			root = NULL;
		}
		
		root = NULL;
	}

	void run(const string& fname);
	
	void addNative(const string& clsName, const string& funcDecl, JSCallback native, void* data);

	BCVar* addClass(const string& clsName, JSCallback nc = NULL);

	inline void loadModule(JSModuleLoader loader) {
		moduleLoader = loader;
		if(moduleLoader != NULL) {
			moduleLoader(this);
		}
	}

	inline JSModuleLoader getModuleLoader() {
		return moduleLoader;
	}

	BCVar* newObject(const string& clsName);

	inline BCVar* getRoot() { return root; }
private:
	JSModuleLoader moduleLoader;
	PC pc;
	PC* code;
	PC codeSize;
	Bytecode bcode;

	BCVar* root;
	vector<VMScope> scopes;

	void exec();

	void init();

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

	/**find node by name in class and superlcasses
	@param name, name of variable;
	*/
	BCNode* findInClass(BCVar* obj, const string& name);


	inline VMScope* scope() { 
		int i = scopes.size() - 1;
		return (i < 0 ? NULL : &scopes[i]);
	}

	//pop stack and release it.
	void pop();

	BCVar* funcDef(const string& funcName);

	void funcCall(const string& funcName);

	void doNew(const string& clsName);

	void doGet(BCVar* v, const string& clsName);

	BCVar* getCurrentObj();

	void compare(OpCode op, BCVar* v1, BCVar* v2);

	void mathOp(OpCode op, BCVar* v1, BCVar* v2);
};

#endif
