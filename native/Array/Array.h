#ifndef JSM_ARRAY
#define JSM_ARRAY

#include "../ClassLoader.h"

namespace JSM {

	class Array : public JSClass {
		static void size(CScriptVar *c, void *);
		static void clear(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "size()", size, NULL);
			addFunction(tinyJS, className, "clear()", clear, NULL);
		}

		public:
		DECL_INSTANCE(Array)
	};

};

#endif
