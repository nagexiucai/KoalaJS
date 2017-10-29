#ifndef JSM_STRING
#define JSM_STRING

#include "../ClassLoader.h"

namespace JSM {

	class String : public JSClass {
		static void indexOf(CScriptVar *c, void *);

		static void substring(CScriptVar *c, void *);

		static void charAt(CScriptVar *c, void *);

		static void charCodeAt(CScriptVar *c, void *);

		static void split(CScriptVar *c, void *);

		static void fromCharCode(CScriptVar *c, void *);

		static void lastIndexOf(CScriptVar *c, void *);

		static void replace(CScriptVar *c, void *);

		static void length(CScriptVar *c, void *);

		static void trimR(CScriptVar *c, void *);

		static void trimL(CScriptVar *c, void *);

		static void trim(CScriptVar *c, void *);

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
