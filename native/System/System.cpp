#include "System.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void System::run(CScriptVar *c, void *userdata) {
	CTinyJS *tinyJS = (CTinyJS *)userdata;
	std::string cmd = c->getParameter("cmd")->getString();

	::system(cmd.c_str());
}

