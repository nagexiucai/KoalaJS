#include "Time.h"
#include <time.h>
#include <sys/time.h>

using namespace std;
using namespace JSM;

void Time::time(KoalaJS* js, BCVar *c, void *userdata) {
	time_t t = ::time(NULL);
	c->getReturnVar()->setInt((int)t);
}

void Time::localTime(KoalaJS* js, BCVar *c, void *userdata) {
	time_t t;
	::time(&t);
	struct tm* lt = localtime(&t);
	
	BCVar* v = new BCVar();
	v->type = BCVar::OBJECT;
	v->addChild("sec", new BCVar((int)lt->tm_sec));
	v->addChild("min", new BCVar((int)lt->tm_min));
	v->addChild("hour", new BCVar((int)lt->tm_hour));
	v->addChild("mday", new BCVar((int)lt->tm_mday));
	v->addChild("mon", new BCVar((int)lt->tm_mon));
	v->addChild("year", new BCVar((int)lt->tm_year));
	v->addChild("wday", new BCVar((int)lt->tm_wday));
	v->addChild("yday", new BCVar((int)lt->tm_yday));
	v->addChild("isdst", new BCVar((int)lt->tm_isdst));
	c->setReturnVar(v);
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
