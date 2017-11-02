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
		BCVar *v = new BCVar("Hello, world! Native");
		v = js.callJSFunc("println", 1, v);
		v->unref();

		v = new BCVar("Hello, world!");
		v = js.callJSFunc("p", 1, v);
		v->unref();
	}

	return 0;
}
