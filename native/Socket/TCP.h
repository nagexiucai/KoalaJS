#ifndef JSM_JSTCP
#define JSM_JSTCP

#include "../ClassLoader.h"

#include <arpa/inet.h> 

namespace JSM {

class JSTCP: public JSClass {
	static void connect(CScriptVar* var, void* data);

	static void close(CScriptVar* var, void* data);

	static void shutdown(CScriptVar* var, void* data);

	static void recv(CScriptVar* var, void* data);

	static void send(CScriptVar* var, void* data);

	static void bind(CScriptVar* var, void* data);

	static void listen(CScriptVar* var, void* data);

	static void accept(CScriptVar* var, void* data);

	protected:
	inline void registerFunctions(KoalaJS* tinyJS, const std::string& className) {
		JSClass::addFunction(tinyJS, className, "close()", close, NULL);
		JSClass::addFunction(tinyJS, className, "shutdown()", shutdown, NULL);
		JSClass::addFunction(tinyJS, className, "connect(host, port, timeout)", connect, NULL);
		JSClass::addFunction(tinyJS, className, "bind(ip, port)", bind, NULL);
		JSClass::addFunction(tinyJS, className, "listen(backlog)", listen, NULL);
		JSClass::addFunction(tinyJS, className, "accept()", accept, tinyJS);
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
