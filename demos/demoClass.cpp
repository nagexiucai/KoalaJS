#include "ClassLoader.h"

static int _count = 0;

//demo how to implement regular class
class MyClass : public JSClass {
	static void test(CScriptVar* var, void* data) {
		printf("Regular class test function %d.\r\n", (*(int*)data)++);
	}	

	protected:
	void registerFunctions(KoalaJS* tinyJS, const std::string& className) {
		addFunction(tinyJS, className, "test()", test, (void*)&_count);
	}

	public:
	DECL_INSTANCE(MyClass)
};


static void moduleLoader(KoalaJS* tinyJS) {
	MyClass::instance().load(tinyJS, "MyClass");
}

int main(int argc, char** argv) {

	while(true) { //Don't be scared, just for memory test:P.
		KoalaJS tinyJS;

		tinyJS.loadModule(moduleLoader);

		tinyJS.exec("var a = new MyClass(); a.test();");
	}

	return 0;
}
