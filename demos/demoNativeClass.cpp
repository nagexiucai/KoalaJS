#include "../TinyJS/ClassLoader.h"


class MyNativeClass {
public:
   void test(CScriptVar* var, void* data) {
     printf("NativeClass test function %d.\r\n", (*(int*)data)++);
   }

	MyNativeClass(void *p) {
	}	
};

static int _count = 0;

template<class T> class MyClass : public NativeClassLoader<T> {

	MAP_FUNC(test)

protected:
	void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
		ClassLoader::addFunction(tinyJS, className, "test()", test, (void*)&_count);
  }

public:
	DECL_INSTANCE(MyClass)
};


int main(int argc, char** argv) {

  while(true) { //Don't be scared, just for memory test:P.
    CTinyJS tinyJS;

    MyClass<MyNativeClass>::instance().load(&tinyJS, "MyClass", NULL);

    tinyJS.execute("var a = new MyClass(); a.test();");
  }

  return 0;
}
