#include "GlobalVars.h"

BCVar* GlobalVars::get(const string& name) {
	locker.lock();
	BCNode* n = global.getChild(name);
	BCVar* var = n == NULL ? NULL : n->var;
	locker.unlock();
	return var;
}

void GlobalVars::set(const string& name, BCVar* v) {
	locker.lock();
	BCNode* n = global.getChildOrCreate(name);
	n->replace(v);
	locker.unlock();
}

