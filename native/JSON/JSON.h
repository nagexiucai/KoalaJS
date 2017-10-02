#ifndef JSM_JSON
#define JSM_JSON

#include "../ClassLoader.h"

namespace JSM {

	class JSON : public JSClass {
		static void stringify(CScriptVar *c, void *);

		static void parse(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "stringify(obj)", stringify, NULL);
			addFunction(tinyJS, className, "parse(src)", parse, tinyJS);
		}

		public:
		DECL_INSTANCE(JSON)
	};

};

#endif
