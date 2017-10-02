#ifndef JSM_JSTCP
#define JSM_JSTCP

#include "../ClassLoader.h"

namespace JSM {

class JSTCPNative {
	int sid;	
public:
	void connect(CScriptVar* var, void* data);

	void close(CScriptVar* var, void* data);

	void shutdown(CScriptVar* var, void* data);

	void recv(CScriptVar* var, void* data);

	void send(CScriptVar* var, void* data);
	
	~JSTCPNative();

	JSTCPNative(void* data);
};


template<class T> 
class JSTCP: public NativeClassLoader<T> {
	MAP_FUNC(connect)
	MAP_FUNC(close)
	MAP_FUNC(recv)
	MAP_FUNC(send)
	MAP_FUNC(shutdown)
	MAP_FUNC(timeout)

	protected:
	inline void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
		JSClass::addFunction(tinyJS, className, "close()", close, NULL);
		JSClass::addFunction(tinyJS, className, "shutdown()", shutdown, NULL);
		JSClass::addFunction(tinyJS, className, "connect(host, port, timeout)", connect, NULL);
		JSClass::addFunction(tinyJS, className, "recv(size, timeout)", recv, tinyJS);
		JSClass::addFunction(tinyJS, className, "send(buf, size, timeout)", send, NULL);
		JSClass::addFunction(tinyJS, className, "read(size)", recv, tinyJS);
		JSClass::addFunction(tinyJS, className, "write(buf, size)", send, NULL);
	}

	public:
	DECL_INSTANCE(JSTCP)
};

};

#endif
