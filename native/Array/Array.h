#ifndef JSM_ARRAY
#define JSM_ARRAY

#include "../ClassLoader.h"

namespace JSM {

	class Array : public JSClass {
		static void size(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "size()", size, NULL);
		}

		public:
		DECL_INSTANCE(Array)
	};

};

#endif
