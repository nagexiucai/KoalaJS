#include "Bytes.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void Bytes::size(KoalaJS* js, CScriptVar* var, void* data) {
	CScriptVar* v = var->getParameter("this");

	var->getReturnVar()->setInt(v->getInt());
}

void Bytes::from(KoalaJS* js, CScriptVar* var, void* data) {
	CScriptVar* sv = var->getParameter("src");
	CScriptVar* v = var->getParameter("this");

	unsigned char* src = NULL;
	int size = 0;

	if(sv->isBytes()) {
		src = (unsigned char*)sv->getPoint();
		size = sv->getInt();
	}
	else {
		std::string s = sv->getString();
		src = (unsigned char*)s.c_str();
		size = s.length();
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


void Bytes::append(KoalaJS* js, CScriptVar* var, void* data) {
	CScriptVar* sv = var->getParameter("src");
	CScriptVar* v = var->getParameter("this");

	unsigned char* src = NULL;
	unsigned char c = 0;
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
	else if(sv->isInt()) {
		c = sv->getInt();
		size = 1;
		src = &c;
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


void Bytes::toString(KoalaJS* js, CScriptVar* var, void* data) {
	CScriptVar* v = var->getParameter("this");
	char* buf = (char*)v->getPoint();
	int size = v->getInt();

	std::string s = "";

	if(size > 0 && buf != NULL)
		s.append(buf, size);
		
	var->getReturnVar()->setString(s);
}

void Bytes::set(KoalaJS* js, CScriptVar* var, void* data) {
	CScriptVar* v = var->getParameter("this");
	int at = var->getParameter("at")->getInt();
	int ch = var->getParameter("ch")->getInt();

	char* buf = (char*)v->getPoint();
	int size = v->getInt();
	
	if(buf == NULL || at < 0 || at >= size) {
		return;
	}
	buf[at] = (unsigned char)ch;
}


