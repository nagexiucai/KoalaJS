#ifndef JSM_VM
#define JSM_VM

#include "../ClassLoader.h"

namespace JSM {

	class VM : public JSClass {
		static void exec(CScriptVar *c, void *);

		static void run(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			/*	addFunction(js, "", "require(cls)", exec, js);
					addFunction(js, "", "exec(src)", exec, js);
					addFunction(js, "", "run(file)", run, js);
					addFunction(js, "", "eval(src)", eval, js); */

			addFunction(js, className, "exec(src)", exec, js);
			addFunction(js, className, "run(file)", run, js);
		}

		public:
		DECL_INSTANCE(VM)
	};

};

#endif
