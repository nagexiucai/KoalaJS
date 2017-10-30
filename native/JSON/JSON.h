#ifndef JSM_JSON
#define JSM_JSON

#include "../ClassLoader.h"

namespace JSM {

	class JSON : public JSClass {
		static void toString(KoalaJS* js, CScriptVar *c, void *);

		static void parse(KoalaJS* js, CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "toString(obj)", toString, NULL);
			addFunction(js, className, "parse(src)", parse, NULL);
		}

		public:
		DECL_INSTANCE(JSON)
	};

};

#endif
