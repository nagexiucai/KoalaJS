#include "VM.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void VM::exec(KoalaJS* js, CScriptVar *c, void *userdata) {
	js->exec(c->getParameter("src")->getString());
}

void VM::run(KoalaJS* js, CScriptVar *c, void *userdata) {
	std::string fname = c->getParameter("file")->getString();

	js->run(fname);
}


