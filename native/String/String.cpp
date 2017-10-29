#include "String.h"
#include "utils/String/StringUtil.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void String::indexOf(CScriptVar *c, void *) {
	string str = c->getParameter("this")->getString();
	string search = c->getParameter("search")->getString();

	size_t p = str.find(search);
	int val = (p==string::npos) ? -1 : p;
	c->getReturnVar()->setInt(val);
}

void String::length(CScriptVar *c, void *) {
	string str = c->getParameter("this")->getString();
	c->getReturnVar()->setInt(str.length());
}

void String::substring(CScriptVar *c, void *) {
	string str = c->getParameter("this")->getString();
	int lo = c->getParameter("lo")->getInt();
	int hi = c->getParameter("hi")->getInt();

	int l = hi-lo;
	if (l>0 && lo>=0 && lo+l<=(int)str.length())
		c->getReturnVar()->setString(str.substr(lo, l));
	else
		c->getReturnVar()->setString("");
}

void String::trimR(CScriptVar *c, void *) {
	string str = c->getParameter("this")->getString();
	c->getReturnVar()->setString(StringUtil::trimRight(str));
}

void String::trim(CScriptVar *c, void *) {
	string str = c->getParameter("this")->getString();
	c->getReturnVar()->setString(StringUtil::trim(str));
}

void String::trimL(CScriptVar *c, void *) {
	string str = c->getParameter("this")->getString();
	c->getReturnVar()->setString(StringUtil::trimLeft(str));
}

void String::charAt(CScriptVar *c, void *) {
	string str = c->getParameter("this")->getString();
	int p = c->getParameter("pos")->getInt();
	if (p>=0 && p<(int)str.length())
		c->getReturnVar()->setString(str.substr(p, 1));
	else
		c->getReturnVar()->setString("");
}

void String::charCodeAt(CScriptVar *c, void *) {
	string str = c->getParameter("this")->getString();
	int p = c->getParameter("pos")->getInt();
	if (p>=0 && p<(int)str.length())
		c->getReturnVar()->setInt(str.at(p));
	else
		c->getReturnVar()->setInt(0);
}

void String::split(CScriptVar *c, void *) {
	string str = c->getParameter("this")->getString();
	string sep = c->getParameter("separator")->getString();
	CScriptVar *result = c->getReturnVar();
	result->setArray();
	int length = 0;

	size_t pos = str.find(sep);
	while (pos != string::npos) {
		result->setArrayIndex(length++, new CScriptVar(str.substr(0,pos)));
		str = str.substr(pos+1);
		pos = str.find(sep);
	}

	if (str.size()>0)
		result->setArrayIndex(length++, new CScriptVar(str));
}

void String::fromCharCode(CScriptVar *c, void *) {
	char str[2];
	str[0] = c->getParameter("char")->getInt();
	str[1] = 0;
	c->getReturnVar()->setString(str);
}

void String::lastIndexOf(CScriptVar *c, void *) {
	string str = c->getParameter("this")->getString();
	string sub = c->getParameter("sub")->getString();

	size_t p = str.rfind(sub);
	int r = (p==string::npos) ? -1 : p;
	c->getReturnVar()->setInt(r);
}

void String::replace(CScriptVar *c, void *) {
	CScriptVar* v = c->getParameter("this");
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

