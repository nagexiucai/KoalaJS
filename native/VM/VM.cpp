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

void VM::getenv(KoalaJS* js, BCVar *c, void *userdata) {
	std::string s = c->getParameter("name")->getString();
	const char* env = ::getenv(s.c_str());
	if(env == NULL)
		s = "";
	else 
		s = env;
	c->setReturnVar(new BCVar(s));
}

void VM::loadExt(KoalaJS* js, BCVar *c, void *userdata) {
	std::string fname = c->getParameter("file")->getString();
	size_t pos = fname.rfind(".so");
	if(pos != fname.length() - 3)
		fname += ".so";
	if(js->loadExt(fname))
		return;

	const char* env = ::getenv("KOALA_ROOT");
	string fn = "/usr/lib/koala";
	if(env != NULL) {
		 fn = env;
		 fn = fn + "/modules";
	}
	//try some lib/koala path
	fn = fn + "/" + fname;
	if(!js->loadExt(fn))
		ERR("Can not load extended module %s!\n", fname.c_str());
}

