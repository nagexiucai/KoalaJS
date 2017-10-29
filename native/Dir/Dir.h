#ifndef JSM_JSDir
#define JSM_JSDir

#include "../ClassLoader.h"

namespace JSM {

class JSDir: public JSClass {
	static void open(CScriptVar* var, void* data);

	static void read(CScriptVar* var, void* data);

	static void close(CScriptVar* var, void* data);

	protected:
	inline void registerFunctions(KoalaJS* js, const std::string& className) {
		addFunction(js, className, "read()", read, NULL);
		addFunction(js, className, "open(name)", open, NULL);
		addFunction(js, className, "close()", close, NULL);
	}

	public:
	DECL_INSTANCE(JSDir)
};

};

#endif
