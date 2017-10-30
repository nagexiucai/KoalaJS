#ifndef JSM_Base64
#define JSM_Base64

#include "../ClassLoader.h"

namespace JSM {

	class JSBase64 : public JSClass {
		static void encode(KoalaJS* js, CScriptVar *c, void *);

		static void decode(KoalaJS* js, CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "encode(src)", encode, NULL);
			addFunction(js, className, "decode(src)", decode, NULL);
		}

		public:
		DECL_INSTANCE(JSBase64)
	};

};

#endif
