#ifndef JSM_GlobalVars
#define JSM_GlobalVars

#include "Var.h"
#include "utils/Thread/Thread.h"

class GlobalVars {
	ThreadLock locker;
	BCVar global;
public:
	void set(const string& name, BCVar* v);

	BCVar* get(const string& name);
};

#endif
