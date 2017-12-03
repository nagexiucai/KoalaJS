#include "VM.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void VM::exec(KoalaJS* js, BCVar *c, void *userdata) {
	std::string src = c->getParameter("src")->getString();
	BCVar* v = c->getParameter("repeat");
	int repeat = true;
	if(v != NULL)
		repeat = v->getInt();
	js->exec(src, repeat==0 ? false:true);
}

void VM::run(KoalaJS* js, BCVar *c, void *userdata) {
	std::string fname = c->getParameter("file")->getString();
	js->run(fname, js->isDebug(), true); //nodebug, repeat
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

void VM::loadJS(KoalaJS* js, BCVar *c, void *userdata) {
	std::string fname = c->getParameter("file")->getString();
	js->run(fname, js->isDebug());
}

void VM::loadClass(KoalaJS* js, BCVar *c, void *userdata) {
	std::string clsName = c->getParameter("cls")->getString();
	std::string fname = c->getParameter("file")->getString();
	BCVar* v = js->loadClass(clsName, fname);
	if(v == NULL)
		return;
	c->setReturnVar(v);
}

void VM::loadModule(KoalaJS* js, BCVar *c, void *userdata) {
	std::string fname = c->getParameter("file")->getString();
	size_t pos = fname.rfind(".so");
	if(pos != fname.length() - 3)
		fname += ".so";
	if(js->loadModule(fname))
		return;

	const char* env = ::getenv("KOALA_ROOT");
	string fn = "/usr/lib/koala/modules";
	if(env != NULL) {
		 fn = env;
		 fn = fn + "/modules";
	}
	//try some lib/koala path
	fn = fn + "/" + fname;
	if(!js->loadModule(fn))
		ERR("Can not load extended module %s!\n", fname.c_str());
}

