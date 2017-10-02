#ifndef JSM_ARRAY
#define JSM_ARRAY

#include "../ClassLoader.h"

namespace JSM {

	class Array : public JSClass {
		static void contains(CScriptVar *c, void *data);

		static void remove(CScriptVar *c, void *data);

		static void join(CScriptVar *c, void *data);

		protected:
		inline void registerFunctions(CTinyJS *tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "contains(obj)", &Array::contains, NULL);
			addFunction(tinyJS, className, "remove(obj)", &Array::remove, NULL);
			addFunction(tinyJS, className, "join(separator)", &Array::join, NULL);
		}
		public:
		DECL_INSTANCE(Array)
	};

};

#endif
