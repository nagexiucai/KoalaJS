#ifndef JSM_System
#define JSM_System

#include "../ClassLoader.h"

namespace JSM {

	class System : public JSClass {
		static void run(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "run(cmd)", run, NULL);
		}

		public:
		DECL_INSTANCE(System)
	};

};

#endif
