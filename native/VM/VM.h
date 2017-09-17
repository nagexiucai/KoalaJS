#ifndef JSM_VM
#define JSM_VM

#include "../../TinyJS/ClassLoader.h"

namespace JSM {

class VM : public ClassLoader {
  static void exec(CScriptVar *c, void *);

  static void eval(CScriptVar *c, void *);

protected:
	void registerFunctions(CTinyJS* tinyJS, const std::string& className);

public:
	DECL_INSTANCE(VM)
};

};

#endif
