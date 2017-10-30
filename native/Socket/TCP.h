#ifndef JSM_JSTCP
#define JSM_JSTCP

#include "../ClassLoader.h"

#include <arpa/inet.h> 

namespace JSM {

class JSTCP: public JSClass {
	static void connect(KoalaJS* js, CScriptVar* var, void* data);

	static void close(KoalaJS* js, CScriptVar* var, void* data);

	static void shutdown(KoalaJS* js, CScriptVar* var, void* data);

	static void recv(KoalaJS* js, CScriptVar* var, void* data);

	static void send(KoalaJS* js, CScriptVar* var, void* data);

	static void bind(KoalaJS* js, CScriptVar* var, void* data);

	static void listen(KoalaJS* js, CScriptVar* var, void* data);

	static void accept(KoalaJS* js, CScriptVar* var, void* data);

	protected:
	inline void registerFunctions(KoalaJS* js, const std::string& className) {
		JSClass::addFunction(js, className, "close()", close, NULL);
		JSClass::addFunction(js, className, "shutdown()", shutdown, NULL);
		JSClass::addFunction(js, className, "connect(host, port, timeout)", connect, NULL);
		JSClass::addFunction(js, className, "bind(ip, port)", bind, NULL);
		JSClass::addFunction(js, className, "listen(backlog)", listen, NULL);
		JSClass::addFunction(js, className, "accept()", accept, NULL);
		JSClass::addFunction(js, className, "recv(size, timeout)", recv, NULL);
		JSClass::addFunction(js, className, "send(buf, size, timeout)", send, NULL);
		JSClass::addFunction(js, className, "read(size)", recv, NULL);
		JSClass::addFunction(js, className, "write(buf, size)", send, NULL);
	}

	public:
	DECL_INSTANCE(JSTCP)
};

};

#endif
