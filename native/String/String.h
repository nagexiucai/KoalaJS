#ifndef JSM_STRING
#define JSM_STRING

#include "../ClassLoader.h"

namespace JSM {

	class String : public JSClass {
		static void indexOf(KoalaJS* js, CScriptVar *c, void *);

		static void substring(KoalaJS* js, CScriptVar *c, void *);

		static void charAt(KoalaJS* js, CScriptVar *c, void *);

		static void charCodeAt(KoalaJS* js, CScriptVar *c, void *);

		static void split(KoalaJS* js, CScriptVar *c, void *);

		static void fromCharCode(KoalaJS* js, CScriptVar *c, void *);

		static void lastIndexOf(KoalaJS* js, CScriptVar *c, void *);

		static void replace(KoalaJS* js, CScriptVar *c, void *);

		static void length(KoalaJS* js, CScriptVar *c, void *);

		static void trimR(KoalaJS* js, CScriptVar *c, void *);

		static void trimL(KoalaJS* js, CScriptVar *c, void *);

		static void trim(KoalaJS* js, CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "indexOf(search)", indexOf, NULL); // find the position of a string in a string, -1 if not
			addFunction(js, className, "substring(lo,hi)", substring, NULL);
			addFunction(js, className, "charAt(pos)", charAt, NULL);
			addFunction(js, className, "charCodeAt(pos)", charCodeAt, NULL);
			addFunction(js, className, "fromCharCode(char)", fromCharCode, NULL);
			addFunction(js, className, "split(separator)", split, NULL);
			addFunction(js, className, "indexOf(sub)", indexOf, NULL);
			addFunction(js, className, "lastIndexOf(sub)", lastIndexOf, NULL);
			addFunction(js, className, "replace(key, with)", replace, NULL);
			addFunction(js, className, "length()", length, NULL);
			addFunction(js, className, "trimR()", trimR, NULL);
			addFunction(js, className, "trimL()", trimL, NULL);
			addFunction(js, className, "trim()", trim, NULL);
		}

		public:
		DECL_INSTANCE(String)
	};

};

#endif
