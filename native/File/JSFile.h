#ifndef JSM_JSFile
#define JSM_JSFile

#include "../ClassLoader.h"

namespace JSM {

#define STDIN "_STDIN"
#define STDOUT "_STDOUT"
#define STDERR "_STDERR"

class JSFile: public JSClass {
	static void size(KoalaJS* js, CScriptVar* var, void* data);

	static void open(KoalaJS* js, CScriptVar* var, void* data);

	static void close(KoalaJS* js, CScriptVar* var, void* data);

	static void read(KoalaJS* js, CScriptVar* var, void* data);

	static void write(KoalaJS* js, CScriptVar* var, void* data);

	static void seek(KoalaJS* js, CScriptVar* var, void* data);

	protected:
	inline void registerFunctions(KoalaJS* js, const std::string& className) {
		JSClass::addConst(js, className, "stdin", new BCVar(STDIN));
		JSClass::addConst(js, className, "stdout", new BCVar(STDOUT));
		JSClass::addConst(js, className, "stderr", new BCVar(STDERR));
		JSClass::addFunction(js, className, "size()", size, NULL);
		JSClass::addFunction(js, className, "close()", close, NULL);
		JSClass::addFunction(js, className, "open(fname, mode)", open, NULL);
		JSClass::addFunction(js, className, "open(fname)", open, NULL);
		JSClass::addFunction(js, className, "constructor(fname)", open, NULL);
		JSClass::addFunction(js, className, "read(size)", read, NULL);
		JSClass::addFunction(js, className, "write(buf, size)", write, NULL);
		JSClass::addFunction(js, className, "write(buf)", write, NULL);
		JSClass::addFunction(js, className, "seek(pos)", seek, NULL);
	}

	public:
	DECL_INSTANCE(JSFile)
};

};

#endif
