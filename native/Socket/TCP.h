#ifndef JSM_JSTCP
#define JSM_JSTCP

#include "../ClassLoader.h"

#include <arpa/inet.h> 

namespace JSM {

class JSTCPNative {
	int sid;	
	struct sockaddr_in addr; 

public:
	void connect(CScriptVar* var, void* data);

	void close(CScriptVar* var, void* data);

	void shutdown(CScriptVar* var, void* data);

	void recv(CScriptVar* var, void* data);

	void send(CScriptVar* var, void* data);

	void bind(CScriptVar* var, void* data);

	void listen(CScriptVar* var, void* data);

	void accept(CScriptVar* var, void* data);

	inline void set(int sid, const struct sockaddr_in& addr) {
		this->sid = sid;
		memcpy(&this->addr, &addr, sizeof(struct sockaddr_in));
	}
	
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
	MAP_FUNC(bind)
	MAP_FUNC(listen)
	MAP_FUNC(accept)

	protected:
	inline void registerFunctions(CTinyJS* tinyJS, const std::string& className) {
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
