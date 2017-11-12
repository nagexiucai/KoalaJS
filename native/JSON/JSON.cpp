#include "JSON.h"
#include "JSONParser.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void JSON::toString(KoalaJS* js, BCVar *c, void *userdata) {
	string result;
	BCVar* v = c->getParameter("pre");
	string pre = "";
		if(v != NULL)
			pre = v->getString();
	result = c->getParameter("obj")->getJSON(pre);
	c->getReturnVar()->setString(result.c_str());
}

void JSON::parse(KoalaJS* js, BCVar *c, void *userdata) {
	string str = c->getParameter("src")->getString();
	BCVar* v = JSONParser::parse(str);
	c->setReturnVar(v);
}

