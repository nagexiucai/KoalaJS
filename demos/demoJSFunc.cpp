#include "ClassLoader.h"
#include "native/Debug/Debug.h"

static void moduleLoader(KoalaJS* js) {
	JSM::Debug::instance().load(js, "Debug");
}

int main(int argc, char** argv) {

	while(true) { //Don't be scared, just for memory test:P.
		KoalaJS js;
		js.loadModule(moduleLoader);
		js.exec("function p(i) { println(i); } ");

		vector<BCVar*> args;
		args.push_back(new BCVar("Hello, world!Native"));
		BCVar* v = js.callJSFunc("println", args);
		v->unref();

		args.push_back(new BCVar("Hello, world!"));
		v = js.callJSFunc("p", args);
		v->unref();
	}

	return 0;
}
