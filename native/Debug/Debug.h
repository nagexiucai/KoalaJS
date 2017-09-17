#ifndef JSM_DEBUG
#define JSM_DEBUG

#include "../../TinyJS/ClassLoader.h"

namespace JSM {

class Debug : public ClassLoader {
  static void print(CScriptVar *v, void *userdata) {
    printf("%s\n", v->getParameter("text")->getString().c_str());
  }

  static void dump(CScriptVar *v, void *userdata) {
    CTinyJS *js = (CTinyJS*)userdata;
    js->root->trace("> ");
  }

protected:
  void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
		ClassLoader::addFunction(tinyJS, "", "print(text)", print, NULL);
		ClassLoader::addFunction(tinyJS, className, "print(text)", print, NULL);
		ClassLoader::addFunction(tinyJS, className, "dump()", dump, NULL);
  }

public:
	DECL_INSTANCE(Debug)

};

};

#endif
