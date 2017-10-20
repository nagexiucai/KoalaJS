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

		protected:
		inline void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "indexOf(search)", indexOf, NULL); // find the position of a string in a string, -1 if not
			addFunction(tinyJS, className, "substring(lo,hi)", substring, NULL);
			addFunction(tinyJS, className, "charAt(pos)", charAt, NULL);
			addFunction(tinyJS, className, "charCodeAt(pos)", charCodeAt, NULL);
			addFunction(tinyJS, className, "fromCharCode(char)", fromCharCode, NULL);
			addFunction(tinyJS, className, "split(separator)", split, NULL);
			addFunction(tinyJS, className, "indexOf(sub)", indexOf, NULL);
			addFunction(tinyJS, className, "lastIndexOf(sub)", lastIndexOf, NULL);
			addFunction(tinyJS, className, "replace(key, with)", replace, NULL);
			addFunction(tinyJS, className, "length()", length, NULL);
		}

		public:
		DECL_INSTANCE(String)
	};

};

#endif
