#include "String.h"
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

void String::registerFunctions(CTinyJS* tinyJS, const std::string& className) {
	ClassLoader::addFunction(tinyJS, className, "indexOf(search)", indexOf, NULL); // find the position of a string in a string, -1 if not
	ClassLoader::addFunction(tinyJS, className, "substring(lo,hi)", substring, NULL);
  ClassLoader::addFunction(tinyJS, className, "charAt(pos)", charAt, NULL);
  ClassLoader::addFunction(tinyJS, className, "charCodeAt(pos)", charCodeAt, NULL);
  ClassLoader::addFunction(tinyJS, className, "fromCharCode(char)", fromCharCode, NULL);
  ClassLoader::addFunction(tinyJS, className, "split(separator)", split, NULL);
}

