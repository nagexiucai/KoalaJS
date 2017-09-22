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
} ThreadDataT;

static void* _vmThread(void* data) {
	ThreadDataT* td = (ThreadDataT*)data;
	if(td == NULL)
		return NULL;

	CTinyJS* tinyJS = td->tinyJS;
	std::string src = td->src;
	delete td; 

	CTinyJS tJS;
	tJS.setModuleLoader(tinyJS->getModuleLoader());
	tJS.loadModule();

	tJS.execute(src);
	return NULL;
}

void JSThread::exec(CScriptVar *c, void *userdata) {
	ThreadDataT *data = new ThreadDataT();

	data->tinyJS = (CTinyJS *)userdata;
	data->src = c->getParameter("src")->getString();

	Thread::run(_vmThread, data);
}

void JSThread::run(CScriptVar *c, void *userdata) {
	ThreadDataT *data = new ThreadDataT();

	data->tinyJS = (CTinyJS *)userdata;
	std::string fname = c->getParameter("file")->getString();
	data->src = FReader::read(fname);
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

void JSThread::registerFunctions(CTinyJS* tinyJS, const std::string& className) {
	addFunction(tinyJS, className, "exec(src)", exec, tinyJS);
	addFunction(tinyJS, className, "run(file)", run, tinyJS);
	addFunction(tinyJS, className, "sleep(sec)", sleep, NULL);
	addFunction(tinyJS, className, "usleep(usec)", usleep, NULL);
}

