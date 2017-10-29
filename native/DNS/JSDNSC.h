#ifndef JSM_DNSC
#define JSM_DNSC

#include "../ClassLoader.h"

namespace JSM {

	class JSDNSC : public JSClass {
		static void resolv(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "resolv(domain, type)", resolv, NULL);
		}

		public:
		DECL_INSTANCE(JSDNSC)
	};

};

#endif
