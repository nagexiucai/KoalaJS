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

protected:
	void registerFunctions(CTinyJS* tinyJS, const std::string& className);

public:
	DECL_INSTANCE(String)
};

};

#endif
