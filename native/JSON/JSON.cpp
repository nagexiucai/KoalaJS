#include "JSON.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void JSON::toString(CScriptVar *c, void *userdata) {
	string result;
	result = c->getParameter("obj")->getJSON();
	c->getReturnVar()->setString(result.c_str());
}

void JSON::parse(CScriptVar *c, void *userdata) {
	CTinyJS *tinyJS = (CTinyJS *)userdata;
	//c->setReturnVar(tinyJS->evaluateComplex(c->getParameter("src")->getString()).var);
}

