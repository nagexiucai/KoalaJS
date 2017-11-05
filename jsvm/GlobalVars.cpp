#include "GlobalVars.h"
#include "utils/Thread/Thread.h"

static ThreadLock _locker;

BCVar* GlobalVars::get(const string& name) {
	_locker.lock();
	BCNode* n = global.getChild(name);
	BCVar* var = n == NULL ? NULL : n->var;
	_locker.unlock();
	return var;
}

void GlobalVars::set(const string& name, BCVar* v) {
	_locker.lock();
	BCNode* n = global.getChildOrCreate(name);
	n->replace(v);
	_locker.unlock();
}

