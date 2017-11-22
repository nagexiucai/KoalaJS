#include "utils/Thread/Thread.h"
#include "KoalaJS.h"
#include "Interrupter.h"
#include <stdarg.h>
#include <stdio.h>

#define INTERUPT_LIMIT 128

static ThreadLock _intrLock;

Interrupter::Interrupter(KoalaJS* h) {
	this->handler = h;
}

void Interrupter::doInterrupt() {
	_intrLock.lock();
	Interrupt* intr = NULL;
	if(!interrupter.empty()) {
		intr = interrupter.front();
		interrupter.pop();
	}
	_intrLock.unlock();

	if(intr == NULL)
		return;
	/*
	TRACE("Interrupted for '%s$%d', %d in queue.\n", 
			intr->funcName.c_str(), 
			(int)intr->args.size(),
			(int)interrupter.size());
	*/

	BCVar* v = handler->callJSFunc(intr->funcName, intr->args);
	v->unref();
	delete intr;
}

void Interrupter::interrupt(const string& funcName, int argNum, ...) {
	_intrLock.lock();
	Interrupt *intr = new Interrupt(funcName);
	//TRACE("get Interrupt %s$%d.\n", funcName.c_str(), argNum);

	va_list args;
	va_start(args, argNum);
	for(int i=0; i<argNum; ++i) {
		BCVar* v = va_arg(args, BCVar*);
		intr->args.push_back(v);
	}
	va_end(args);

	size_t sz = interrupter.size();
	if(sz >= INTERUPT_LIMIT) { //drop oldest one if too much request.
		TRACE("Too much interrupter (%d), drop the oldest.\n", (int)sz);
		Interrupt* it = interrupter.front();
		interrupter.pop();
		size_t num = it->args.size();
		for(size_t i=0; i<num; ++i) {
			delete it->args[i];
		}
		delete it;
	}
	interrupter.push(intr);
	_intrLock.unlock();
}


