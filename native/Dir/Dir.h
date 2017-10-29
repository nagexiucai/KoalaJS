#ifndef JSM_JSDir
#define JSM_JSDir

#include "../ClassLoader.h"
#include <dirent.h>

namespace JSM {

class JSDirNative {
	DIR* dir;	
public:
	void open(CScriptVar* var, void* data);

	void close(CScriptVar* var, void* data);

	void read(CScriptVar* var, void* data);

	~JSDirNative();

	JSDirNative(void* data);
};


template<class T> 
class JSDir: public NativeClassLoader<T> {
	MAP_FUNC(open)
	MAP_FUNC(close)
	MAP_FUNC(read)

	protected:
	inline void registerFunctions(KoalaJS* tinyJS, const std::string& className) {
		JSClass::addFunction(tinyJS, className, "close()", close, NULL);
		JSClass::addFunction(tinyJS, className, "open(name)", open, NULL);
		JSClass::addFunction(tinyJS, className, "read()", read, NULL);
	}

	public:
	DECL_INSTANCE(JSDir)
};

};

#endif
