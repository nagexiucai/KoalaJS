#ifndef JSM_GlobalVars
#define JSM_GlobalVars

#include "Var.h"

class GlobalVars {
	BCVar global;
public:
	void set(const string& name, BCVar* v);

	BCVar* get(const string& name);
};

#endif
