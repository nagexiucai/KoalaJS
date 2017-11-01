#ifndef JSM_MD5
#define JSM_MD5

#include "../ClassLoader.h"

namespace JSM {

	class JSMD5 : public JSClass {
		static std::string hashString(const unsigned char* s, unsigned int size);

		static std::string hashHex(const unsigned char* s, unsigned int size);

		static void hashString(KoalaJS* js, BCVar *c, void *);

		static void hashHex(KoalaJS* js, BCVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "hashString(src)", hashString, NULL);
			addFunction(js, className, "hashHex(src)", hashHex, NULL);
		}

		public:
		DECL_INSTANCE(JSMD5)
	};

};

#endif
