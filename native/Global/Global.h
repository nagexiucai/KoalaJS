#ifndef JSM_Global
#define JSM_Global

#include "../ClassLoader.h"
#include "libs/Thread/Thread.h"

namespace JSM {

	class Global : public JSClass {

		static void set(CScriptVar *c, void *);

		static void get(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "get(name)", get, NULL);
			addFunction(tinyJS, className, "set(name, value)", set, NULL);
		}

		public:
		DECL_INSTANCE(Global)
	};

};

#endif
