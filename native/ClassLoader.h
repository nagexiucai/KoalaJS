#ifndef JS_CLASS_LOADER_HH
#define JS_CLASS_LOADER_HH

#include "KoalaJS.h"

using namespace std;

#define DECL_INSTANCE(T) static T& instance() { static T t; return t; }

class JSClass {
	protected:
		void addFunction(KoalaJS* tinyJS, const std::string& clsName, const std::string& decl, JSCallback ptr, void* data) {
			tinyJS->addNative(clsName, decl, ptr, data);
		}
		
		void addConst(KoalaJS* tinyJS, const std::string& clsName, const std::string& name, BCVar* v) {
			BCVar* cls = tinyJS->getOrAddClass(clsName);
			cls->addChild(name, v, true);
		}

		virtual void registerFunctions(KoalaJS* tinyJS, const std::string& className)  = 0;

	public: 
		void load(KoalaJS* tinyJS, const std::string& clsName) {
			registerFunctions(tinyJS, clsName);
		}
    virtual ~JSClass() {}
};


#endif
