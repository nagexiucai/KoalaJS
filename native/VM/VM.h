#ifndef JSM_VM
#define JSM_VM

#include "../ClassLoader.h"

namespace JSM {

	class VM : public JSClass {
		static void exec(KoalaJS* js, BCVar *c, void *);

		static void run(KoalaJS* js, BCVar *c, void *);

		static void loadModule(KoalaJS* js, BCVar *c, void *);

		static void loadJS(KoalaJS* js, BCVar *c, void *);

		static void loadClass(KoalaJS* js, BCVar *c, void *);

		static void getenv(KoalaJS* js, BCVar *c, void *);

		static void require(KoalaJS* js, BCVar *c, void *);

		static void terminate(KoalaJS* js, BCVar *c, void *);

		static void terminated(KoalaJS* js, BCVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "terminate()", terminate, NULL);
			addFunction(js, className, "terminated()", terminated, NULL);
			addFunction(js, className, "exec(src,repeat)", exec, NULL);
			addFunction(js, className, "exec(src)", exec, NULL);
			addFunction(js, className, "run(file)", run, NULL);
			addFunction(js, className, "getenv(name)", getenv, NULL);
			addFunction(js, className, "loadModule(file)", loadModule, NULL);
			addFunction(js, className, "loadJS(file)", loadJS, NULL);
			addFunction(js, className, "loadClass(cls, file)", loadClass, NULL);
			addFunction(js, className, "require(file)", require, NULL);
			addFunction(js, "", "require(file)", require, NULL);
		}

		public:
		DECL_INSTANCE(VM)
	};

};

#endif
