#ifndef JSM_JSFile
#define JSM_JSFile

#include "../ClassLoader.h"

namespace JSM {

class JSFileNative {
	int fid;	
public:
	void size(CScriptVar* var, void* data);

	void open(CScriptVar* var, void* data);

	void close(CScriptVar* var, void* data);

	void read(CScriptVar* var, void* data);

	void write(CScriptVar* var, void* data);

	void seek(CScriptVar* var, void* data);

	~JSFileNative();

	JSFileNative(void* data);
};


template<class T> 
class JSFile: public NativeClassLoader<T> {
	MAP_FUNC(size)
	MAP_FUNC(open)
	MAP_FUNC(close)
	MAP_FUNC(read)
	MAP_FUNC(write)
	MAP_FUNC(seek)

	protected:
	void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
		JSClass::addFunction(tinyJS, className, "size()", size, NULL);
		JSClass::addFunction(tinyJS, className, "close()", close, NULL);
		JSClass::addFunction(tinyJS, className, "open(fname, mode)", open, NULL);
		JSClass::addFunction(tinyJS, className, "read(size)", read, NULL);
		JSClass::addFunction(tinyJS, className, "write(buf, size)", write, NULL);
		JSClass::addFunction(tinyJS, className, "seek(pos)", seek, NULL);
	}

	public:
	DECL_INSTANCE(JSFile)
};

};

#endif
