#include "VM.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void VM::exec(CScriptVar *c, void *userdata) {
    CTinyJS *tinyJS = (CTinyJS *)userdata;
    tinyJS->execute(c->getParameter("src")->getString());
}

void VM::eval(CScriptVar *c, void *userdata) {
    CTinyJS *tinyJS = (CTinyJS *)userdata;
    c->setReturnVar(tinyJS->evaluateComplex(c->getParameter("src")->getString()).var);
}

void VM::registerFunctions(CTinyJS* tinyJS, const std::string& className) {
  addFunction(tinyJS, "", "exec(src)", exec, tinyJS);
  addFunction(tinyJS, "", "eval(src)", eval, tinyJS);
  addFunction(tinyJS, className, "exec(src)", exec, tinyJS);
  addFunction(tinyJS, className, "eval(src)", eval, tinyJS);
}

