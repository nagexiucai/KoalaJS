#include "TCP.h"
#include "libs/DNS/DNS.h"
#include <cstdlib>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <sys/types.h>


using namespace std;
using namespace JSM;


JSTCPNative::~JSTCPNative() {
	if(sid >= 0) {
		::close(sid);
	}

	memset(&addr, 0, sizeof(addr)); 
	sid = -1;
}

JSTCPNative::JSTCPNative(void* data) {
	sid = socket(PF_INET, SOCK_STREAM, 0);
}

void JSTCPNative::close(CScriptVar* var, void* data) {
	if(sid >= 0) {
		::close(sid);
	}

	memset(&addr, 0, sizeof(addr)); 
	sid = -1;
}

void JSTCPNative::shutdown(CScriptVar* var, void* data) {
	if(sid >= 0)
		::shutdown(sid, SHUT_RDWR);
}

void JSTCPNative::send(CScriptVar* var, void* data) {
	CScriptVar* v = var->getParameter("buf");
	int size = var->getParameter("size")->getInt();
	int to = var->getParameter("timeout")->getInt();

	if(size <= 0 || sid < 0 || !v->isBytes())
		return;

	if(size > v->getInt())
		size = v->getInt();

	char* p = (char*)v->getPoint();

	v = var->getReturnVar();
 	v->setInt(-1);	

	struct timeval tv_timeout;
	tv_timeout.tv_sec = to;
	tv_timeout.tv_usec = 0;

	if(setsockopt(sid, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv_timeout, sizeof(struct timeval)) < 0) {
		return;
	}

	if(p != NULL) {
		size = ::write(sid, p, size);
		v->setInt(size);
	}
}


void JSTCPNative::recv(CScriptVar* var, void* data) {
	int size = var->getParameter("size")->getInt();
	int to = var->getParameter("timeout")->getInt();
	CTinyJS* tinyJS = (CTinyJS*)data;

	if(size <= 0 || sid < 0)
		return;
	
	char* buf = new char[size];
	if(buf == NULL)
		return;

	struct timeval tv_timeout;
	tv_timeout.tv_sec = to;
	tv_timeout.tv_usec = 0;

	if(setsockopt(sid, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv_timeout, sizeof(struct timeval)) < 0) {
		return;
	}

	size = ::read(sid, buf, size);
	if(size < 0) {
		delete []buf;
		return;
	}
	
	CScriptVar* v = tinyJS->newObject("Bytes"); 
	v->setPoint(buf, size, NULL, true);
	var->setReturnVar(v);
}

void JSTCPNative::bind(CScriptVar* var, void* data) {
	std::string ip = var->getParameter("ip")->getString();
	int port = var->getParameter("port")->getInt();

	var->getReturnVar()->setInt(0);

	if(sid < 0 || port <= 0)
		return;

	memset(&addr, 0, sizeof(addr)); 
	addr.sin_family = AF_INET; 
	addr.sin_port=htons(port);

	if(ip.length() > 0)
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
	else
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(::bind(sid,(struct sockaddr *)&addr,sizeof(struct sockaddr)) < 0) {
		return;
	}

	var->getReturnVar()->setInt(1);
}

void JSTCPNative::accept(CScriptVar* var, void* data) {
	CTinyJS* tinyJS = (CTinyJS*)data;
	if(sid < 0)
		return;

	struct sockaddr_in in;
	memset(&addr, 0, sizeof(in)); 
	in.sin_family = AF_INET; 
	socklen_t size = sizeof(struct sockaddr);

	int cid = ::accept(sid,(struct sockaddr *)&in, &size);
	if(cid < 0) {
		TRACE("TCP accept failed, errno: %d\n", errno);
		return;
	}
	
	CScriptVar* obj = tinyJS->newObject("RTCP");
	JSTCPNative* nobj = (JSTCPNative*)obj->getPoint();
	if(nobj == NULL) {
		obj->unref();
		return;
	}

	nobj->set(cid, in);
	var->setReturnVar(obj);
}

void JSTCPNative::listen(CScriptVar* var, void* data) {
	int backlog = var->getParameter("backlog")->getInt();

	var->getReturnVar()->setInt(0);

	if(sid < 0)
		return;

	if(::listen(sid, backlog) < 0) {
		return;
	}

	var->getReturnVar()->setInt(1);
}

void JSTCPNative::connect(CScriptVar* var, void* data) {
	std::string host = var->getParameter("host")->getString();
	int port = var->getParameter("port")->getInt();
	int to = var->getParameter("timeout")->getInt();

	var->getReturnVar()->setInt(0);

	host = DNS::ip(host);
	if(host.length() == 0 || port <= 0)
		return;

	memset(&addr, 0, sizeof(addr)); 
	addr.sin_family = AF_INET; 
	addr.sin_addr.s_addr = inet_addr(host.c_str());
	addr.sin_port=htons(port);


	if(to <= 0) {
		if(::connect(sid,(struct sockaddr *)&addr,sizeof(struct sockaddr)) < 0) {
			return;
		}
	}
	else {

		unsigned long ul = 1;
		ioctl(sid, FIONBIO, &ul);

		if(::connect(sid,(struct sockaddr *)&addr,sizeof(struct sockaddr)) < 0) {
			int error=-1, len;

			timeval tm;
			tm.tv_sec = to;
			tm.tv_usec = 0;

			fd_set set;
			FD_ZERO(&set);
			FD_SET(sid, &set);

			if( select(sid+1, NULL, &set, NULL, &tm) > 0) {
				getsockopt(sid, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
				if(error != 0) 
					return;
			}
		}

		ul = 0;
		ioctl(sid, FIONBIO, &ul); 
	}

	var->getReturnVar()->setInt(1);
}

