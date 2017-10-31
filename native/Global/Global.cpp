#include "Global.h"
#include <cstdlib>
#include <stdio.h>
#include <map>

using namespace std;
using namespace JSM;

void Global::get(KoalaJS* js, CScriptVar *c, void *userdata) {
	std::string name = c->getParameter("name")->getString();
	BCVar* v = KoalaJS::getGlobalVars()->get(name);
	if(v != NULL)
		c->setReturnVar(v);
}

void Global::set(KoalaJS* js, CScriptVar *c, void *userdata) {
	std::string name = c->getParameter("name")->getString();
	CScriptVar* v = c->getParameter("value");
	
	KoalaJS::getGlobalVars()->set(name, v);
}


