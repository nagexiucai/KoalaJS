#include "../native/ClassLoader.h"

//demo how to implement native class
class MyNativeClass {
	public:
		void test(CScriptVar* var, void* data) {
			printf("NativeClass test function %d.\r\n", (*(int*)data)++);
		}

		MyNativeClass(void *p) {
		}	
};

static int _count = 0;

template<class T> class MyNativeClassLoader : public NativeClassLoader<T> {

	MAP_FUNC(test)

	protected:
		void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
			JSClass::addFunction(tinyJS, className, "test()", test, (void*)&_count);
		}

	public:
		DECL_INSTANCE(MyNativeClassLoader)
};

//demo how to implement regular class
class MyClass : public JSClass {
	static void test(CScriptVar* var, void* data) {
		printf("Regular class test function %d.\r\n", (*(int*)data)++);
	}	

	protected:
	void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
		addFunction(tinyJS, className, "test()", test, (void*)&_count);
	}

	public:
	DECL_INSTANCE(MyClass)
};


int main(int argc, char** argv) {

	while(true) { //Don't be scared, just for memory test:P.
		CTinyJS tinyJS;

		MyClass::instance().load(&tinyJS, "MyClass");
		MyNativeClassLoader<MyNativeClass>::instance().load(&tinyJS, "MyNativeClass", NULL);

		tinyJS.execute("var a = new MyNativeClass(); a.test(); var b = new MyClass(); b.test();");
	}

	return 0;
}
