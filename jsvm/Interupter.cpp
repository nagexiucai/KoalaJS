#include "utils/Thread/Thread.h"
#include "KoalaJS.h"
#include "Interupter.h"
#include <stdarg.h>
#include <stdio.h>

#define INTERUPT_LIMIT 128

static ThreadLock _intrLock;

Interupter::Interupter(KoalaJS* h) {
	this->handler = h;
}

void Interupter::doInterupt() {
	_intrLock.lock();
	Interupt* intr = NULL;
	if(!interupter.empty()) {
		intr = interupter.front();
		interupter.pop();
	}
	_intrLock.unlock();

	if(intr == NULL)
		return;

	TRACE("Interupted for '%s$%d', %d in queue.\n", 
			intr->funcName.c_str(), 
			(int)intr->args.size(),
			(int)interupter.size());

	BCVar* v = handler->callJSFunc(intr->funcName, intr->args);
	v->unref();
	delete intr;
}

void Interupter::interupt(const string& funcName, int argNum, ...) {
	_intrLock.lock();
	Interupt *intr = new Interupt(funcName);
	TRACE("get Interupt %s$%d.\n", funcName.c_str(), argNum);

	va_list args;
	va_start(args, argNum);
	for(int i=0; i<argNum; ++i) {
		BCVar* v = va_arg(args, BCVar*);
		intr->args.push_back(v);
	}
	va_end(args);

	size_t sz = interupter.size();
	if(sz >= INTERUPT_LIMIT) { //drop oldest one if too much request.
		TRACE("Too much interupter (%d), drop the oldest.\n", (int)sz);
		Interupt* it = interupter.front();
		interupter.pop();
		size_t num = it->args.size();
		for(size_t i=0; i<num; ++i) {
			delete it->args[i];
		}
		delete it;
	}
	interupter.push(intr);
	_intrLock.unlock();
}


