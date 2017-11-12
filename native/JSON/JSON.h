#ifndef JSM_JSON
#define JSM_JSON

#include "../ClassLoader.h"

namespace JSM {

	class JSON : public JSClass {
		static void toString(KoalaJS* js, BCVar *c, void *);

		static void parse(KoalaJS* js, BCVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "toString(obj)", toString, NULL);
			addFunction(js, className, "toString(obj, pre)", toString, NULL);
			addFunction(js, className, "parse(src)", parse, NULL);
		}

		public:
		DECL_INSTANCE(JSON)
	};

};

#endif
