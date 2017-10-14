#include "Var.h"

BCNode::BCNode(const string& n, BCVar* v) {
	var = v->ref();
	name = n;
	beConst = false;
	isNode = true;
}

BCNode::~BCNode() {
	if(var != NULL)
		var->unref();
}

void BCNode::replace(BCVar* v) {
	BCVar* old = var;
	var = v->ref();
	old->unref();
}

STFunc::STFunc() {
	args = new BCVar();
}

STFunc::~STFunc() {
	delete args;
}

void STFunc::resetArgs() {
	int sz = argNum;
	if(args->getChild(THIS) != NULL)
		sz++;

	BCVar* v = new BCVar();
	for(int i=0; i<sz; ++i) {
		BCNode* n = args->getChild(i);
		if(n)
			n->replace(v);
	}
}

