#include "ClassLoader.h"
#include "native/Debug/Debug.h"

static int _count = 0;

//demo how to implement regular class
class MyClass : public JSClass {
	static void test(KoalaJS* js, CScriptVar* var, void* data) {
		printf("Regular class test function %d.\r\n", (*(int*)data)++);
	}	

	protected:
	void registerFunctions(KoalaJS* js, const std::string& className) {
		addFunction(js, className, "test()", test, (void*)&_count);
	}

	public:
	DECL_INSTANCE(MyClass)
};

static void moduleLoader(KoalaJS* js) {
	MyClass::instance().load(js, "MyClass");
}

int main(int argc, char** argv) {

	while(true) { //Don't be scared, just for memory test:P.
		KoalaJS js;
		js.loadModule(moduleLoader);
		js.exec("var a = new MyClass(); a.test();");
	}

	return 0;
}
