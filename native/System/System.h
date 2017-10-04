#ifndef JSM_System
#define JSM_System

#include "../ClassLoader.h"

namespace JSM {

	class System : public JSClass {
		static void run(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "run(cmd)", run, tinyJS);
		}

		public:
		DECL_INSTANCE(System)
	};

};

#endif
