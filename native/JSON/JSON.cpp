#include "JSON.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void JSON::toString(KoalaJS* js, CScriptVar *c, void *userdata) {
	string result;
	result = c->getParameter("obj")->getJSON();
	c->getReturnVar()->setString(result.c_str());
}

void JSON::parse(KoalaJS* js, CScriptVar *c, void *userdata) {
	//c->setReturnVar(js->evaluateComplex(c->getParameter("src")->getString()).var);
}

