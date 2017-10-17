#include "Thread.h"
#include "libs/Thread/Thread.h"
#include "libs/File/File.h"
#include <cstdlib>
#include <unistd.h>

using namespace std;
using namespace JSM;

typedef struct ThreadData {
	CTinyJS* tinyJS;
	std::string src;
	bool code;
	CScriptVar* arg;
} ThreadDataT;

static void* _vmThread(void* data) {
	ThreadDataT* td = (ThreadDataT*)data;
	if(td == NULL)
		return NULL;

	CTinyJS* tinyJS = td->tinyJS;
	std::string src = td->src;
	bool code = td->code;
	CScriptVar* arg = td->arg;

	delete td;
	pthread_detach(pthread_self());

	CTinyJS *tJS = new CTinyJS();
	tJS->loadModule(tinyJS->getModuleLoader());
	tJS->setcwd(tinyJS->getcwd());
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

void JSThread::exec(CScriptVar *c, void *userdata) {
	ThreadDataT *data = new ThreadDataT();

	data->tinyJS = (CTinyJS *)userdata;
	data->src = c->getParameter("src")->getString();
	data->code = true;
	data->arg = c->getParameter("arg")->ref();

	Thread::run(_vmThread, data);
}

void JSThread::run(CScriptVar *c, void *userdata) {
	ThreadDataT *data = new ThreadDataT();

	data->tinyJS = (CTinyJS *)userdata;
	data->src = c->getParameter("file")->getString();
	data->code = false;
	data->arg = c->getParameter("arg")->ref();
	Thread::run(_vmThread, data);
}

void JSThread::sleep(CScriptVar *c, void *userdata) {
	int sec = c->getParameter("sec")->getInt();
	::sleep(sec);
}

void JSThread::usleep(CScriptVar *c, void *userdata) {
	int sec = c->getParameter("usec")->getInt();
	::usleep(sec);
}

