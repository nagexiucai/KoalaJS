#ifndef JSM_DEBUG
#define JSM_DEBUG

#include "../ClassLoader.h"
#include <stdio.h>

namespace JSM {

	class Debug : public JSClass {
		static void print(CScriptVar *v, void *userdata) {
			string s = v->getParameter("text")->getString();
			printf("%s", s.c_str());
		}

		static void println(CScriptVar *v, void *userdata) {
			string s = v->getParameter("text")->getString() + "\n";
			printf("%s", s.c_str());
		}

		static void dump(CScriptVar *v, void *userdata) {
			CTinyJS *js = (CTinyJS*)userdata;
			//js->root->trace("> ");
		}

		protected:
		void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, "", "print(text)", print, NULL);
			addFunction(tinyJS, "", "println(text)", println, NULL);
			addFunction(tinyJS, className, "print(text)", print, NULL);
			addFunction(tinyJS, className, "println(text)", println, NULL);
			addFunction(tinyJS, className, "dump()", dump, tinyJS);
		}

		public:
		DECL_INSTANCE(Debug)

	};

};

#endif
