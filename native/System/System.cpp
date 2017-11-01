#include "System.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void System::run(KoalaJS* js, BCVar *c, void *userdata) {
	std::string cmd = c->getParameter("cmd")->getString();
	//::system(cmd.c_str());
}

