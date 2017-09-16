#include "TinyJS/TinyJS.h"


class MyNativeClass {
public:
   void test() {
     printf("NativeClass test function.\r\n");
   }

	MyNativeClass(void *p) {
	}	
};


template<class T> class NativeClass {
  static void destroy(void* p) {
    delete (T*)p;
  }

  static void constructor(CScriptVar* var, void *userData) {
    void* nativeClass = new T(userData);
    var->setPoint(nativeClass, destroy, true);
  }

public:
	static T* getClass(CScriptVar* var) {
    T* cls = (T*)var->getPoint();
		return cls;	
	}

  static void registerClass(CTinyJS* tinyJS, const std::string& className, void* constructData ) {
    CScriptVar* cls = new CScriptVar(TINYJS_BLANK_DATA, SCRIPTVAR_OBJECT);
    cls->setNativeConstructor(constructor, constructData);
    tinyJS->addClass(className, cls);
	}

	static void addNative(CTinyJS* tinyJS, const std::string& className, const std::string& decl, int argnum, JSCallback ptr) {
		std::string s = "function ";
		s += className + "." + decl;
    tinyJS->addNative(s, ptr, 0);
	}

  static void load(CTinyJS* tinyJS, const std::string& className, void* constructData) {
		NativeClass<T>::registerClass(tinyJS, className, constructData);
		registerFunctions(tinyJS, className);
	}

	static void registerFunctions(CTinyJS* tinyJS, const std::string& className) { }
	
};


template<class T> class MyClass : public NativeClass<T> {

 static void test (CScriptVar* var, void *userData) {
    T* cls = NativeClass<T>::getClass(var);
    cls->test();
  }


public:
	static void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
		NativeClass<T>::addNative(tinyJS, className, "test()", 0, test);
  }
};


int main(int argc, char** argv) {

  while(true) { //Don't be scared, just for memory test:P.
    CTinyJS tinyJS;

    MyClass<MyNativeClass>::load(&tinyJS, "MyClass", NULL);

    tinyJS.execute("var a = new MyClass(); a.test();");
  }

  return 0;
}
