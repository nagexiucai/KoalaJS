#include "VM.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void VM::exec(KoalaJS* js, BCVar *c, void *userdata) {
	KoalaJS kjs(js->getRoot(), js->getInterrupter());
	kjs.exec(c->getParameter("src")->getString());
}

void VM::run(KoalaJS* js, BCVar *c, void *userdata) {
	std::string fname = c->getParameter("file")->getString();

	KoalaJS kjs(js->getRoot(), js->getInterrupter());
	kjs.run(fname);
}

void VM::loadExt(KoalaJS* js, BCVar *c, void *userdata) {
	std::string fname = c->getParameter("file")->getString();
	size_t pos = fname.rfind(".so");
	if(pos != fname.length() - 3)
		fname += ".so";
	if(!js->loadExt(fname)) {
		//try /usr/local/lib/koala path
		string fn = "/usr/local/lib/koala/";
		fname = fn + fname;
		if(!js->loadExt(fname))
			ERR("Can not load extended module %s!\n", fname.c_str());
	}
}



