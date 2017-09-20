#ifndef JSM_Base64
#define JSM_Base64

#include "../ClassLoader.h"

namespace JSM {

	class JSBase64 : public JSClass {
		static void encode(CScriptVar *c, void *);

		static void decode(CScriptVar *c, void *);

		protected:
		void registerFunctions(CTinyJS* tinyJS, const std::string& className);

		public:
		DECL_INSTANCE(JSBase64)
	};

};

#endif
