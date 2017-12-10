#include "VM.h"
#include "utils/File/File.h"
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
#ifdef KOALA_DEBUG
	js->run(fname, js->isDebug(), true); //nodebug, repeat
#else
	js->run(fname, false, true); //nodebug, repeat
#endif
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
#ifdef KOALA_DEBUG
	js->run(fname, js->isDebug());
#else
	js->run(fname, false);
#endif
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

void VM::require(KoalaJS* js, BCVar* c, void *userdata) {
	std::string fname = c->getParameter("file")->getString();

	BCVar* module = new BCVar();
	module->type = BCVar::OBJECT;
	BCVar* exports = new BCVar();
	exports->type = BCVar::OBJECT;
	module->addChild("exports", exports);

	c->addChild("module", module);
	BCNode* expNode = c->addChild("exports", exports);

	string fn = File::getFullname(js->getcwd(), fname);
	if(!File::exist(fn)) {
		const char* env = ::getenv("KOALA_ROOT");
		fn = "/usr/lib/koala/classes";
		if(env != NULL) {
			fn = env;
			fn = fn + "/classes";
		}
		//try some lib/koala path
		fn = fn + "/" + fname;
		if(!File::exist(fn))  {
			ERR("Required module %s not exist!\n", fname.c_str());
			return;
		}
	}

#ifdef KOALA_DEBUG
	js->run(fn, js->isDebug(), true);
#else
	js->run(fn, false, true);
#endif
	c->setReturnVar(expNode->var);
}

