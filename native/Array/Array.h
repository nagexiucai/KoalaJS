#ifndef JSM_ARRAY
#define JSM_ARRAY

#include "../../TinyJS/ClassLoader.h"

namespace JSM {

class Array : public JSClass {
  static void contains(CScriptVar *c, void *data);

  static void remove(CScriptVar *c, void *data);

  static void join(CScriptVar *c, void *data);

protected:
 	void registerFunctions(CTinyJS* tinyJS, const std::string& classname);
public:
	DECL_INSTANCE(Array)
};

};

#endif
