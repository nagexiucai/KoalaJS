#include "VM.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void VM::exec(CScriptVar *c, void *userdata) {
	KoalaJS *js = (KoalaJS *)userdata;
	js->exec(c->getParameter("src")->getString());
}

void VM::run(CScriptVar *c, void *userdata) {
	KoalaJS *js = (KoalaJS *)userdata;
	std::string fname = c->getParameter("file")->getString();

	js->run(fname);
}


