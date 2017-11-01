#ifndef JSM_DEBUG
#define JSM_DEBUG

#include "../ClassLoader.h"
#include <stdio.h>

namespace JSM {

	class Debug : public JSClass {
		static void print(KoalaJS* js, BCVar *v, void *userdata) {
			string s = v->getParameter("text")->getString();
			printf("%s", s.c_str());
		}

		static void println(KoalaJS* js, BCVar *v, void *userdata) {
			string s = v->getParameter("text")->getString() + "\n";
			printf("%s", s.c_str());
		}

		static void dump(KoalaJS* js, BCVar *v, void *userdata) {
			string s = js->getRoot()->getString();
			printf("%s", s.c_str());
		}

		protected:
		void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, "", "print(text)", print, NULL);
			addFunction(js, "", "println(text)", println, NULL);
			addFunction(js, className, "print(text)", print, NULL);
			addFunction(js, className, "println(text)", println, NULL);
			addFunction(js, className, "dump()", dump, js);
		}

		public:
		DECL_INSTANCE(Debug)

	};

};

#endif
