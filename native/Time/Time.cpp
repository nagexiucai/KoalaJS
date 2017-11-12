#include "Time.h"
#include <time.h>
#include <sys/time.h>

using namespace std;
using namespace JSM;

void Time::time(KoalaJS* js, BCVar *c, void *userdata) {
	time_t t = ::time(NULL);
	c->getReturnVar()->setInt((int)t);
}

void Time::timeOfDay(KoalaJS* js, BCVar *c, void *userdata) {
	struct timeval tv;
	struct timezone tz;

	::gettimeofday(&tv, &tz);
	BCVar* v = new BCVar();
	v->type = BCVar::OBJECT;
	v->addChild("sec", new BCVar((int)tv.tv_sec));
	v->addChild("usec", new BCVar((int)tv.tv_usec));
	c->setReturnVar(v);
}
