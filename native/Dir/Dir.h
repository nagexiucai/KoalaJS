#ifndef JSM_JSDir
#define JSM_JSDir

#include "../ClassLoader.h"

namespace JSM {

class JSDir: public JSClass {
	static void open(CScriptVar* var, void* data);

	static void read(CScriptVar* var, void* data);

	static void close(CScriptVar* var, void* data);

	protected:
	inline void registerFunctions(KoalaJS* tinyJS, const std::string& className) {
		addFunction(tinyJS, className, "read()", read, NULL);
		addFunction(tinyJS, className, "open(name)", open, NULL);
		addFunction(tinyJS, className, "close()", close, NULL);
	}

	public:
	DECL_INSTANCE(JSDir)
};

};

#endif
