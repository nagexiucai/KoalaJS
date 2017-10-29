#ifndef JSM_JSON
#define JSM_JSON

#include "../ClassLoader.h"

namespace JSM {

	class JSON : public JSClass {
		static void toString(CScriptVar *c, void *);

		static void parse(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "toString(obj)", toString, NULL);
			addFunction(tinyJS, className, "parse(src)", parse, tinyJS);
		}

		public:
		DECL_INSTANCE(JSON)
	};

};

#endif
