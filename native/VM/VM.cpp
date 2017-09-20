#include "VM.h"
#include "libs/Thread/Thread.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void VM::exec(CScriptVar *c, void *userdata) {
	CTinyJS *tinyJS = (CTinyJS *)userdata;
	tinyJS->execute(c->getParameter("src")->getString());
}

void VM::eval(CScriptVar *c, void *userdata) {
	CTinyJS *tinyJS = (CTinyJS *)userdata;
	c->setReturnVar(tinyJS->evaluateComplex(c->getParameter("src")->getString()).var);
}


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

	tinyJS->execute(src);
	return NULL;
}

void VM::thread(CScriptVar *c, void *userdata) {
	ThreadDataT *data = new ThreadDataT();

	data->tinyJS = (CTinyJS *)userdata;
	data->src = c->getParameter("src")->getString();

	Thread::run(_vmThread, data);
}

void VM::registerFunctions(CTinyJS* tinyJS, const std::string& className) {
	addFunction(tinyJS, "", "exec(src)", exec, tinyJS);
	addFunction(tinyJS, "", "eval(src)", eval, tinyJS);
	addFunction(tinyJS, className, "exec(src)", exec, tinyJS);
	addFunction(tinyJS, className, "eval(src)", eval, tinyJS);
	addFunction(tinyJS, className, "thread(src)", thread, tinyJS);
}

