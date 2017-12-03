#include "String.h"
#include "utils/String/StringUtil.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void String::indexOf(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	string search = c->getParameter("search")->getString();

	size_t p = str.find(search);
	int val = (p==string::npos) ? -1 : (int)p;
	c->getReturnVar()->setInt(val);
}

void String::length(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	c->getReturnVar()->setInt((int)str.length());
}

void String::substring(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	int lo = c->getParameter("lo")->getInt();
	int hi = c->getParameter("hi")->getInt();

	int l = hi-lo;
	if (l>0 && lo>=0 && lo+l<=(int)str.length())
		c->getReturnVar()->setString(str.substr(lo, l));
	else
		c->getReturnVar()->setString("");
}

void String::trimR(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	c->getReturnVar()->setString(StringUtil::trimRight(str));
}

void String::trim(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	c->getReturnVar()->setString(StringUtil::trim(str));
}

void String::trimL(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	c->getReturnVar()->setString(StringUtil::trimLeft(str));
}

void String::charAt(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	int p = c->getParameter("pos")->getInt();
	if (p>=0 && p<(int)str.length())
		c->getReturnVar()->setString(str.substr(p, 1));
	else
		c->getReturnVar()->setString("");
}

void String::charCodeAt(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	int p = c->getParameter("pos")->getInt();
	if (p>=0 && p<(int)str.length())
		c->getReturnVar()->setInt(str.at(p));
	else
		c->getReturnVar()->setInt(0);
}

void String::split(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	string sep = c->getParameter("separator")->getString();
	BCVar *result = c->getReturnVar();
	result->setArray();
	int length = 0;

	size_t pos = str.find(sep);
	while (pos != string::npos) {
		result->setArrayIndex(length++, new BCVar(str.substr(0,pos)));
		str = str.substr(pos+1);
		pos = str.find(sep);
	}

	if (str.size()>0)
		result->setArrayIndex(length++, new BCVar(str));
}

void String::fromCharCode(KoalaJS* js, BCVar *c, void *) {
	char str[2];
	str[0] = c->getParameter("char")->getInt();
	str[1] = 0;
	c->getReturnVar()->setString(str);
}

void String::lastIndexOf(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	string sub = c->getParameter("search")->getString();

	size_t p = str.rfind(sub);
	int r = (p==string::npos) ? -1 : (int)p;
	c->getReturnVar()->setInt(r);
}

void String::replace(KoalaJS* js, BCVar *c, void *) {
	BCVar* v = c->getParameter("this");
	string str = v->getString();
	string key = c->getParameter("key")->getString();
	string with = c->getParameter("with")->getString();

	if(key.length() == 0) {
		c->getReturnVar()->setString(str);
		return;
	}

	string s = "";

	while(true) {
		size_t i = str.find(key);
		if(i == string::npos) {
			s += str;
			break;
		}
		else {
			s += str.substr(0, i) + with;
			str = str.substr(i+key.length());
		}
	}

	v->setString(s);	
	c->getReturnVar()->setString(s);
}

void String::toInt(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	int i;

	if(str.find("0x") != string::npos ||
			str.find("0x") != string::npos) 
		i = strtol(str.c_str(), NULL, 16);
	else
		i = strtol(str.c_str(), NULL, 10);
	c->setReturnVar(new BCVar(i));	
}	

void String::toFloat(KoalaJS* js, BCVar *c, void *) {
	string str = c->getParameter("this")->getString();
	float f;

	f = atof(str.c_str());
	c->setReturnVar(new BCVar(f));	
}

void String::format(KoalaJS* js, BCVar *c, void *) {
	char buf[32] = "";
	string str = c->getParameter("fmt")->getString();
	BCVar* v = c->getParameter("val");
	if(v->type == BCVar::INT)
		snprintf(buf, 32, str.c_str(), v->getInt());
	else if(v->type == BCVar::FLOAT)
		snprintf(buf, 32, str.c_str(), v->getFloat());

	c->setReturnVar(new BCVar(buf));	
}		
