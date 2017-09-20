#ifndef JSM_MD5
#define JSM_MD5

#include "../ClassLoader.h"

namespace JSM {

	class JSMD5 : public JSClass {
		static std::string hashString(const unsigned char* s, unsigned int size);

		static std::string hashHex(const unsigned char* s, unsigned int size);

		static void hashString(CScriptVar *c, void *);

		static void hashHex(CScriptVar *c, void *);

		protected:
		void registerFunctions(CTinyJS* tinyJS, const std::string& className);

		public:
		DECL_INSTANCE(JSMD5)
	};

};

#endif
