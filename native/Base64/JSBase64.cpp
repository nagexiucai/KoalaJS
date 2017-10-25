#include "JSBase64.h"
#include "libs/Base64/Base64.h"

using namespace std;
using namespace JSM;

void JSBase64::encode(CScriptVar *c, void *userdata) {
	std::string s = "";
	CScriptVar* v = c->getParameter("src");
	
	if(v->isString()) {
		s = v->getString();
		s = Base64::encode((const unsigned char*)s.c_str(), s.length());
	}
	else if(v->isBytes()) {
		unsigned char* p = (unsigned char*)v->getPoint();
		int size = v->getInt();
		
		if(size > 0 && p != NULL)
			s = Base64::encode(p, size);
	}

	c->getReturnVar()->setString(s);
}

void JSBase64::decode(CScriptVar *c, void *userdata) {
	std::string src = c->getParameter("src")->getString();
	CTinyJS* tinyJS = (CTinyJS*)userdata;

	size_t size = 0;
	unsigned char* p = Base64::decode(src, size);

	CScriptVar* v = tinyJS->newObject("Bytes"); 
	v->setPoint(p, size, NULL, true);
	c->setReturnVar(v);
}

