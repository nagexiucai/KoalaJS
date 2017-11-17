#include "Array.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void Array::size(KoalaJS* js, BCVar *c, void *) {
	int i = c->getParameter("this")->getArrayLength();
	c->getReturnVar()->setInt(i);
}

void Array::clear(KoalaJS* js, BCVar *c, void *) {
	c->getParameter("this")->removeAllChildren();
}

void Array::add(KoalaJS* js, BCVar *c, void *) {
	BCVar* thisV = c->getParameter("this");
	BCVar* v = c->getParameter("v");
	thisV->addChild("", v);

	c->setReturnVar(v);
}

