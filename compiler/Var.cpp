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

