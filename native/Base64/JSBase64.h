#ifndef JSM_Base64
#define JSM_Base64

#include "../ClassLoader.h"

namespace JSM {

	class JSBase64 : public JSClass {
		static void encode(CScriptVar *c, void *);

		static void decode(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "encode(src)", encode, NULL);
			addFunction(tinyJS, className, "decode(src)", decode, tinyJS);
		}

		public:
		DECL_INSTANCE(JSBase64)
	};

};

#endif
