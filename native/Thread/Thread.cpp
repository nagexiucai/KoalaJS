#include "Thread.h"
#include "utils/Thread/Thread.h"
//#include "utils/File/File.h"
#include <cstdlib>
#include <unistd.h>

using namespace std;
using namespace JSM;

typedef struct ThreadData {
	KoalaJS* js;
	std::string src;
	bool code;
	CScriptVar* arg;
} ThreadDataT;

static void* _vmThread(void* data) {
	ThreadDataT* td = (ThreadDataT*)data;
	if(td == NULL)
		return NULL;

	KoalaJS* js = td->js;
	std::string src = td->src;
	bool code = td->code;
	CScriptVar* arg = td->arg;

	delete td;
	pthread_detach(pthread_self());

	KoalaJS *tJS = new KoalaJS();
	tJS->loadModule(js->getModuleLoader());
	tJS->setcwd(js->getcwd());
	tJS->getRoot()->addChild("_threadArg", arg);
	arg->unref();

	if(code)
		tJS->exec(src);
	else {
		tJS->run(src);
	}
	
	delete tJS;
	return NULL;
}

void JSThread::exec(KoalaJS* js, CScriptVar *c, void *userdata) {
	ThreadDataT *data = new ThreadDataT();

	data->js = js;
	data->src = c->getParameter("src")->getString();
	data->code = true;
	data->arg = c->getParameter("arg")->ref();

	Thread::run(_vmThread, data);
}

void JSThread::run(KoalaJS* js, CScriptVar *c, void *userdata) {
	ThreadDataT *data = new ThreadDataT();

	data->js = js;
	data->src = c->getParameter("file")->getString();
	data->code = false;
	data->arg = c->getParameter("arg")->ref();
	Thread::run(_vmThread, data);
}

void JSThread::sleep(KoalaJS* js, CScriptVar *c, void *userdata) {
	int sec = c->getParameter("sec")->getInt();
	::sleep(sec);
}

void JSThread::usleep(KoalaJS* js, CScriptVar *c, void *userdata) {
	int sec = c->getParameter("usec")->getInt();
	::usleep(sec);
}

