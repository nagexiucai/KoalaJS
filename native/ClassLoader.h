#ifndef JS_CLASS_LOADER_HH
#define JS_CLASS_LOADER_HH

#include "KoalaJS.h"

using namespace std;

#define DECL_INSTANCE(T) static T& instance() { static T t; return t; }

#define MAP_FUNC(func) static inline void func (CScriptVar* var, void *userData) { \
	T* cls = NativeClassLoader<T>::getClass(var); \
	cls->func(var, userData); \
}

class JSClass {
	protected:
		void addFunction(KoalaJS* tinyJS, const std::string& className, const std::string& decl, JSCallback ptr, void* data) {
			tinyJS->addNative(className, decl, ptr, data);
		}

		virtual void registerFunctions(KoalaJS* tinyJS, const std::string& className)  = 0;

	public: 
		void load(KoalaJS* tinyJS, const std::string& className) {
			registerFunctions(tinyJS, className);
		}

};

template<class T> class NativeClassLoader: public JSClass {
	static void destroy(void* p) {
		delete (T*)p;
	}

	static void constructor(CScriptVar* var, void *userData) {
		void* nativeClass = new T(userData);
		var->setPoint(nativeClass, -1, destroy, true);
	}

	protected:
	static T* getClass(CScriptVar* var) {
		CScriptVar* v = var->getParameter("this");

		T* cls = (T*)v->getPoint();
		return cls;	
	}

	static void registerClass(KoalaJS* tinyJS, const std::string& className) {
		tinyJS->addClass(className, constructor);
	}

	public:
	void load(KoalaJS* tinyJS, const std::string& className) {
		NativeClassLoader<T>::registerClass(tinyJS, className);
		registerFunctions(tinyJS, className);
	}
};


#endif
