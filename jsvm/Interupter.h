#ifndef BC_INTERUPTS_HH
#define BC_INTERUPTS_HH

#include "Var.h"
#include <queue>

typedef struct STInterupt {
	string funcName;
	vector<BCVar*> args;

	inline STInterupt(const string& n) {
		funcName = n;
	}
} Interupt;

class KoalaJS;

class Interupter {
	queue<Interupt*> interupter;
	KoalaJS* handler;
public:
	Interupter(KoalaJS* h);

	void doInterupt();
	
	void interupt(const string& funcName, int argNum, ...); 
};


#endif
