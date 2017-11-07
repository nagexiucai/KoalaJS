#ifndef JSM_DEBUG
#define JSM_DEBUG

#include "../ClassLoader.h"
#include <unistd.h>

namespace JSM {

	class Debug : public JSClass {
		static int getFD(KoalaJS* js) {
			BCNode* n = js->getRoot()->getChildOrCreate("_debugFD");
			return n->var->getInt();
		}
		
		static void setFD(KoalaJS* js, BCVar* v, void* data) {
			int fd = v->getParameter("fd")->getInt();
			BCNode* n = js->getRoot()->getChildOrCreate("_debugFD");
			n->var->setInt(fd);
		}

		static void print(KoalaJS* js, BCVar *v, void *userdata) {
			string s = v->getParameter("text")->getString();
			int fd = Debug::getFD(js);
			::write(fd, s.c_str(), s.length());
		}

		static void println(KoalaJS* js, BCVar *v, void *userdata) {
			string s = v->getParameter("text")->getString() + "\n";
			int fd = Debug::getFD(js);
			::write(fd, s.c_str(), s.length());
		}

		static void dump(KoalaJS* js, BCVar *v, void *userdata) {
			string s = js->getRoot()->getString();
			int fd = Debug::getFD(js);
			::write(fd, s.c_str(), s.length());
		}

		protected:
		void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, "", "print(text)", print, NULL);
			addFunction(js, "", "println(text)", println, NULL);
			addFunction(js, className, "print(text)", print, NULL);
			addFunction(js, className, "println(text)", println, NULL);
			addFunction(js, className, "setFD(fd)", setFD, NULL);
			addFunction(js, className, "dump()", dump, js);
		}

		public:
		DECL_INSTANCE(Debug)

	};

};

#endif
