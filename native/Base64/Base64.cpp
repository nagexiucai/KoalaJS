#include "Base64.h"
#include "libs/Base64/Base64.h"

using namespace std;
using namespace JSM;

void JSBase64::encode(CScriptVar *c, void *userdata) {
	std::string src = c->getParameter("src")->getString();
	src = Base64::encode((const unsigned char*)src.c_str(), src.length());

	c->getReturnVar()->setString(src);
}

void JSBase64::decode(CScriptVar *c, void *userdata) {
	std::string src = c->getParameter("src")->getString();
	src = Base64::decode(src);

	c->getReturnVar()->setString(src);
}

void JSBase64::registerFunctions(CTinyJS* tinyJS, const std::string& className) {
	addFunction(tinyJS, className, "encode(src)", encode, NULL);
	addFunction(tinyJS, className, "decode(src)", decode, NULL);
}

