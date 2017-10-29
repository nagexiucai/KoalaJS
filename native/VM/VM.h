#ifndef JSM_VM
#define JSM_VM

#include "../ClassLoader.h"

namespace JSM {

	class VM : public JSClass {
		static void exec(CScriptVar *c, void *);

		static void run(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* tinyJS, const std::string& className) {
			/*	addFunction(tinyJS, "", "require(cls)", exec, tinyJS);
					addFunction(tinyJS, "", "exec(src)", exec, tinyJS);
					addFunction(tinyJS, "", "run(file)", run, tinyJS);
					addFunction(tinyJS, "", "eval(src)", eval, tinyJS); */

			addFunction(tinyJS, className, "exec(src)", exec, tinyJS);
			addFunction(tinyJS, className, "run(file)", run, tinyJS);
		}

		public:
		DECL_INSTANCE(VM)
	};

};

#endif
