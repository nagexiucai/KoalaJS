#ifndef BC_KoalaJS_HH
#define BC_KoalaJS_HH

#include "Bytecode.h"
#include "CodeCache.h"
#include "Var.h"
#include "GlobalVars.h"
#include "Debug.h"
#include "Interrupter.h"
#include <stack>
#include <map>
#include <dlfcn.h>

#define VAR(i) (i->isNode ? ((BCNode*)i)->var : (BCVar*)i)

typedef struct STScope {
	BCVar* var;
	PC pc; //stack pc
	inline STScope() {
		var = NULL;
		pc = 0xFFFFFFFF;
	}
} VMScope;

typedef struct {
	PC pc;
	PC *code;
	PC size;
	Bytecode* bcode;
} CodeT;

class KoalaJS;
typedef void (*JSModuleLoader)(KoalaJS *js);

class KoalaJS {
	public:
		inline KoalaJS() {
			moduleLoader = NULL;
			pc = 0;
			codeSize = 0;
			code = NULL;
			bcode = NULL;
			stackTop = STACK_DEEP;
			this->root = NULL;
			interrupter = new Interrupter(this);
			freeInterrupter = true;
			debugMode = false;
			init();
		}
		
		inline KoalaJS(BCVar* rt, Interrupter* inter) {
			moduleLoader = NULL;
			pc = 0;
			codeSize = 0;
			code = NULL;
			bcode = NULL;
			stackTop = STACK_DEEP;
			interrupter = inter;
			freeInterrupter = false;
			debugMode = false;
			init(rt);
		}

		inline ~KoalaJS() {
			clear();
			//unload extended lib.
			std::map<string, void*>::iterator it;
			for(it=extDL.begin(); it!=extDL.end(); ++it) {
				void* h = it->second;
				if(h != NULL)
					dlclose(h);
			}
			if(freeInterrupter)
				delete interrupter;
		}

		inline void reset() {
			clear();
			init();
			loadModule(moduleLoader);
		}

		inline void clear() {
			bcode = NULL;
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
			stackTop = STACK_DEEP;

			if(root != NULL) {
				root->unref();
				root = NULL;
			}
		}

		inline bool isDebug() {
			return debugMode;
		}

		bool run(const string& fname, bool debug=false, bool repeat=false);

		bool exec(const string& code, bool repeat=true);

		void addNative(const string& clsName, const string& funcDecl, JSCallback native, void* data);

		BCVar* getOrAddClass(const string& clsName);

		BCVar* loadClass(const string& clsName, const string& jsFile);

		inline void loadModule(JSModuleLoader loader) {
			moduleLoader = loader;
			if(moduleLoader != NULL) {
				moduleLoader(this);
			}
			loadModules();
		}

		inline JSModuleLoader getModuleLoader() {
			return moduleLoader;
		}

		bool loadModule(const string& fname);

		BCVar* newObject(BCNode* cls);

		BCVar* newObject(const string& clsName);

		inline BCVar* getRoot() { return root; }

		inline const std::string& getcwd() { return cwd; }

		inline void setcwd(const std::string& cwd) { this->cwd = cwd; }

		static GlobalVars* getGlobalVars();

		BCNode* load(const string& name, bool create = false);

		/**Call JS Function , Warning! don't call it from diff thread.
			@param name, function name.
			@param argNum, argument number.
			@param ..., arguments (BCVar*) type.
			@return BCVar* from JS Function return, you have to unref() it !!
		 */
		BCVar* callJSFunc(const string& name, int argNum, ...);

		BCVar* callJSFunc(const string& name, const vector<BCVar*>& args, BCVar* obj = NULL);

		bool prepareFuncArgs(int argNum, vector<BCVar*>& args);

		void releaseFuncArgs(vector<BCVar*>& args);

		void interrupt(const string& name, int argNum, ...);

		inline void stop() {
			pc = codeSize + 1;
		}

		inline Bytecode* getBytecode() {
			return bcode;
		}

		inline int scopeDeep() {
			return (int)scopes.size();
		}

		inline Interrupter* getInterrupter() {
			return interrupter;
		}
	private:
		string cwd;
		string cname;

		JSModuleLoader moduleLoader;
		PC pc;
		PC* code;
		PC codeSize;
		Bytecode* bcode;

		BCVar* root;
		vector<VMScope> scopes;
		map<string, void*> extDL; //extended module(dynamic lib)
		map<string, PC> extJSC; //extended js classes
		stack<CodeT> codeStack; //code stack for "run" or "exec" js in js.
		//run stack
		const static uint16_t STACK_DEEP = 128;
		StackItem* vStack[STACK_DEEP];
		uint16_t stackTop;
		Debug debug;
		bool debugMode;

		//interrupt queue	
		Interrupter* interrupter;
		bool freeInterrupter;

		void loadModules();

		void doInterrupt();

		void runCode(Bytecode* bc, PC startPC = 0);

		void init(BCVar* rt = NULL);

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
			int i = ((int)scopes.size()) - 1;
			return (i < 0 ? NULL : &scopes[i]);
		}

		//pop stack and release it.
		void pop();

		inline void setBytecode(Bytecode* bc) {
			bcode = bc;
			code = bcode->getCode(codeSize);
		}

		BCVar* funcDef(const string& funcName, bool regular = true);

		BCNode* findFunc(BCVar* owner, const string& funcName, bool member);

		BCNode* findMemberFunc(BCVar* owner, const string& funcName);

		BCVar* runJSFunc(BCVar* obj, BCVar* funcV, const vector<BCVar*>* args);

		bool funcCall(BCVar* object, BCVar* func, const vector<BCVar*>* args = NULL);

		void doNew(const string& clsName);

		void doExtends(BCVar* v, const string& clsName);

		bool construct(BCVar* obj, int argNum);

		void doGet(BCVar* v, const string& clsName);

		BCVar* getCurrentObj(bool create = false);

		void compare(OprCode op, BCVar* v1, BCVar* v2);

		void mathOp(OprCode op, BCVar* v1, BCVar* v2);

		inline void pushScope(const VMScope& sc) {
			scopes.push_back(sc);
		}

		inline void popScope() {
			VMScope* sc = scope();
			if(sc->pc != 0xFFFFFFFF)
				pc = sc->pc;

			if(sc != NULL && sc->var != NULL) {
				sc->var->unref();
			}
			scopes.pop_back();
		}
};

#endif
