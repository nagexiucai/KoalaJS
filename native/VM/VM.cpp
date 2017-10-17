#include "VM.h"
#include "libs/File/File.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void VM::exec(CScriptVar *c, void *userdata) {
	CTinyJS *tinyJS = (CTinyJS *)userdata;
	tinyJS->exec(c->getParameter("src")->getString());
}

void VM::run(CScriptVar *c, void *userdata) {
	CTinyJS *tinyJS = (CTinyJS *)userdata;
	std::string fname = c->getParameter("file")->getString();

	tinyJS->run(fname);
}


