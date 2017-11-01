#include "Global.h"
#include "GlobalVars.h"
#include <cstdlib>
#include <stdio.h>
#include <map>

using namespace std;
using namespace JSM;

void Global::get(KoalaJS* js, BCVar *c, void *userdata) {
	std::string name = c->getParameter("name")->getString();
	BCVar* v = KoalaJS::getGlobalVars()->get(name);
	if(v != NULL)
		c->setReturnVar(v);
}

void Global::set(KoalaJS* js, BCVar *c, void *userdata) {
	std::string name = c->getParameter("name")->getString();
	BCVar* v = c->getParameter("value");
	
	KoalaJS::getGlobalVars()->set(name, v);
}


