#include "JSON.h"
#include "JSONParser.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void JSON::toString(KoalaJS* js, BCVar *c, void *userdata) {
	string result;
	result = c->getParameter("obj")->getJSON();
	c->getReturnVar()->setString(result.c_str());
}

void JSON::parse(KoalaJS* js, BCVar *c, void *userdata) {
	string str = c->getParameter("src")->getString();
	BCVar* v = JSONParser::parse(str);
	c->setReturnVar(v);
}

