#include "ClassLoader.h"
#include "native/Debug/Debug.h"

static void moduleLoader(KoalaJS* js) {
	JSM::Debug::instance().load(js, "Debug");
}

int main(int argc, char** argv) {
	KoalaJS js;
	js.loadModule(moduleLoader);
	js.exec("function p(i) { println(i); } ");

	while(true) { //Don't be scared, just for memory test:P.
		vector<BCVar*> args;
		BCVar* v;
		args.push_back(new BCVar("Hello, world!Native"));
		v = js.callJSFunc("println", args);
		v->unref();

		args.push_back(new BCVar("Hello, world!"));
		v = js.callJSFunc("p", args);
		v->unref();
	}

	return 0;
}
