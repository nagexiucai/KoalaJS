#include "Timer.h"
#include "utils/Thread/Thread.h"
#include <unistd.h>


using namespace std;
using namespace JSM;

static uint32_t _timerUSEC = 0xFFFFFFFF;

#define MINI_STEP 20000 //mini step as 0.02 sec.
static void* _timerThread(void* data) {
	Interrupter* inter = (Interrupter*)data;	
	pthread_detach(pthread_self());

	uint32_t count = 0;
	while(_timerUSEC != 0xFFFFFFFF) {
		::usleep(MINI_STEP);
		count++;
		if(_timerUSEC > 0) {
			if(count > (_timerUSEC / MINI_STEP)) {
				count = 0;
				inter->interrupt(NULL, "_onTimer", 0);
			}
		}
	}
	_timerUSEC = 0xFFFFFFFF;
	return NULL;
}

void Timer::on(KoalaJS* js, BCVar *c, void *userdata) {
	if(_timerUSEC == 0xFFFFFFFF) {
		_timerUSEC = 0;
		Thread::run(_timerThread, js->getInterrupter());
	}
}

void Timer::off(KoalaJS* js, BCVar *c, void *userdata) {
	_timerUSEC = 0xFFFFFFFF;
}

void Timer::set(KoalaJS* js, BCVar *c, void *userdata) {
	int usec = c->getParameter("usec")->getInt();
	_timerUSEC = usec;
}

