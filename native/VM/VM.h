#ifndef JSM_VM
#define JSM_VM

#include "../ClassLoader.h"

namespace JSM {

	class VM : public JSClass {
		static void exec(KoalaJS* js, BCVar *c, void *);

		static void run(KoalaJS* js, BCVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "exec(src)", exec, NULL);
			addFunction(js, className, "run(file)", run, NULL);
		}

		public:
		DECL_INSTANCE(VM)
	};

};

#endif
