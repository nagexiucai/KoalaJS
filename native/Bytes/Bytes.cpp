#include "Bytes.h"
#include "libs/File/File.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void Bytes::size(CScriptVar* var, void* data) {
	CScriptVar* v = var->getParameter("this");

	var->getReturnVar()->setInt(v->getInt());
}

void Bytes::from(CScriptVar* var, void* data) {
	
	CScriptVar* sv = var->getParameter("src");
	CScriptVar* v = var->getParameter("this");

	unsigned char* src = NULL;
	int size = 0;

	if(sv->isString()) {
		std::string s = sv->getString();
		src = (unsigned char*)s.c_str();
		size = s.length();
	}
	else if(sv->isBytes()) {
		src = (unsigned char*)sv->getPoint();
		size = sv->getInt();
	}

	unsigned char* buf = NULL;

	if(size > 0 && src != NULL) {
		buf = new unsigned char[size];	
		if(buf == NULL)
			size = 0;
		else
			memcpy(buf, src, size);
	}

	v->setPoint(buf, size, NULL, true);	
	var->setReturnVar(v);
}


void Bytes::append(CScriptVar* var, void* data) {
	CScriptVar* sv = var->getParameter("src");
	CScriptVar* v = var->getParameter("this");

	unsigned char* src = NULL;
	int size = 0;

	if(sv->isString()) {
		std::string s = sv->getString();
		src = (unsigned char*)s.c_str();
		size = s.length();
	}
	else if(sv->isBytes()) {
		src = (unsigned char*)sv->getPoint();
		size = sv->getInt();
	}
	
	int oldSize = v->getInt();
	unsigned char* oldP = (unsigned char*)v->getPoint();
	unsigned char* buf = NULL;

	if(size > 0 && src != NULL) {
		buf = new unsigned char[size + oldSize];	
		
		if(buf != NULL) {
			if(oldSize > 0 && oldP != NULL)
				memcpy(buf, oldP, oldSize);

			memcpy(buf+oldSize, src, size);

			v->setPoint(buf, size+oldSize, NULL, true);	
		}
	}

	var->setReturnVar(v);
}


void Bytes::toString(CScriptVar* var, void* data) {
	CScriptVar* v = var->getParameter("this");
	char* buf = (char*)v->getPoint();
	int size = v->getInt();

	std::string s = "";

	if(size > 0 && buf != NULL)
		s.append(buf, size);
		
	var->getReturnVar()->setString(s);
}


void Bytes::registerFunctions(CTinyJS* tinyJS, const std::string& className) {
	addFunction(tinyJS, className, "size()", size, NULL);
	addFunction(tinyJS, className, "from(src)", from, NULL);
	addFunction(tinyJS, className, "append(src)", append, NULL);
	addFunction(tinyJS, className, "toString()", toString, NULL);
}

