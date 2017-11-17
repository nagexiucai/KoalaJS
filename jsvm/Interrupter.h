#ifndef BC_INTERRUPTS_HH
#define BC_INTERRUPTS_HH

#include "Var.h"
#include <queue>

typedef struct STInterrupt {
	string funcName;
	vector<BCVar*> args;

	inline STInterrupt(const string& n) {
		funcName = n;
	}
} Interrupt;

class KoalaJS;

class Interrupter {
	queue<Interrupt*> interrupter;
	KoalaJS* handler;
public:
	Interrupter(KoalaJS* h);

	void doInterrupt();
	
	void interrupt(const string& funcName, int argNum, ...); 
};


#endif
