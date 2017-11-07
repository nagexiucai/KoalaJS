#include "TCP.h"
#include "utils/DNS/dnsc.h"
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

#define DEF_TIMEOUT		60 //default time out 60 sec

typedef struct {
	int sid;	
	struct sockaddr_in addr; 
} SocketT;

static void destroySocket(void* data) {
	SocketT* sc = (SocketT*)data;
	if(sc != NULL && sc->sid >= 0)
		::close(sc->sid);
	delete sc;
}

static BCVar* newSocketObj(int sid, const struct sockaddr_in* addr) {
	SocketT* sc = new SocketT();
	sc->sid = sid;
	if(addr != NULL)
		memcpy(&sc->addr, addr, sizeof(struct sockaddr_in));

	BCVar* v = new BCVar();
	v->setPoint(sc, NO_BYTES, destroySocket, true);
	return v;
}

static BCVar* setSocket(BCVar* var, int sid, const struct sockaddr_in* addr) {
	BCVar* thisV = var->getParameter("this");
	if(thisV == NULL)
		return NULL;

	BCVar* v = newSocketObj(sid, addr);
	BCNode* n = thisV->getChildOrCreate("socket");
	n->replace(v);
	return thisV;
}

static SocketT* getSocket(BCVar* var) {
	BCVar* thisV = var->getParameter("this");
	if(thisV == NULL)
		return NULL;
	
	BCNode* n = thisV->getChild("socket");
	if(n == NULL)
		return NULL;
	return (SocketT*)(n->var->getPoint());
}

void JSTCP::close(KoalaJS* js, BCVar* var, void* data) {
	setSocket(var, -1, NULL);
}

void JSTCP::getFD(KoalaJS* js, BCVar* var, void* data) {
	var->setReturnVar(new BCVar(-1));

	SocketT* sc = getSocket(var);
	if(sc != NULL)
		var->setReturnVar(new BCVar(sc->sid));
}

void JSTCP::shutdown(KoalaJS* js, BCVar* var, void* data) {
	SocketT* sc = getSocket(var);
	if(sc != NULL && sc->sid >= 0)
		::shutdown(sc->sid, SHUT_RDWR);
}

void JSTCP::send(KoalaJS* js, BCVar* var, void* data) {
	int size = -1;
	BCVar* v = var->getParameter("buf");
	BCVar* n = var->getParameter("size");
	if(n != NULL)
		size = var->getParameter("size")->getInt();

	n = var->getParameter("timeout");
	int to = DEF_TIMEOUT;
	if(n != NULL)
		to = n->getInt();

	SocketT* sc = getSocket(var);
	if(sc == NULL || sc->sid < 0)
		return;

	if(sc->sid < 0)
		return;

	char* p = NULL;
	int sz = -1;
	string s;
	if(v->isBytes()) {
		p = (char*)v->getPoint();
		sz = v->getInt();
	}
	else {
		s = v->getString();
		p = (char*)s.c_str();
		sz = s.length();
	}
	if(size < 0 || size > sz)
		size = sz;

	if(size <= 0)
		return;

	v = var->getReturnVar();
 	v->setInt(-1);	

	struct timeval tv_timeout;
	tv_timeout.tv_sec = to;
	tv_timeout.tv_usec = 0;

	if(setsockopt(sc->sid, SOL_SOCKET, SO_SNDTIMEO, (void *) &tv_timeout, sizeof(struct timeval)) < 0) {
		return;
	}
	if(p != NULL) {
		size = (int)::write(sc->sid, p, size);
		v->setInt(size);
	}
}


void JSTCP::recv(KoalaJS* js, BCVar* var, void* data) {
	int size = var->getParameter("size")->getInt();
	BCVar* n = var->getParameter("timeout");
	int to = DEF_TIMEOUT;
	if(n != NULL)
		to = n->getInt();

	SocketT* sc = getSocket(var);
	if(sc == NULL || sc->sid < 0 || size <= 0)
		return;

	char* buf = new char[size];
	if(buf == NULL)
		return;

	struct timeval tv_timeout;
	tv_timeout.tv_sec = to;
	tv_timeout.tv_usec = 0;

	if(setsockopt(sc->sid, SOL_SOCKET, SO_RCVTIMEO, (void *) &tv_timeout, sizeof(struct timeval)) < 0) {
		return;
	}

	size = (int)::read(sc->sid, buf, size);
	if(size < 0) {
		delete []buf;
		return;
	}
	
	BCVar* v = js->newObject("Bytes"); 
	v->setPoint(buf, size, NULL, true);
	var->setReturnVar(v);
}

void JSTCP::bind(KoalaJS* js, BCVar* var, void* data) {
	std::string ip = var->getParameter("ip")->getString();
	int port = var->getParameter("port")->getInt();

	var->getReturnVar()->setInt(0);

	int sid = socket(PF_INET, SOCK_STREAM, 0);
	if(sid < 0 || port <= 0)
		return;

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr)); 
	addr.sin_family = AF_INET; 
	addr.sin_port=htons(port);

	if(ip.length() > 0)
		addr.sin_addr.s_addr = inet_addr(ip.c_str());
	else
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(::bind(sid,(struct sockaddr *)&addr,sizeof(struct sockaddr)) < 0) {
		::close(sid);
		return;
	}
	setSocket(var, sid, NULL);

	var->getReturnVar()->setInt(1);
}

void JSTCP::accept(KoalaJS* js, BCVar* var, void* data) {
	SocketT* sc = getSocket(var);
	if(sc == NULL || sc->sid < 0)
		return;

	struct sockaddr_in in;
	memset(&in, 0, sizeof(in)); 
	in.sin_family = AF_INET; 
	socklen_t size = sizeof(struct sockaddr);

	int cid = ::accept(sc->sid,(struct sockaddr *)&in, &size);
	if(cid < 0) {
		return;
	}
	
	BCVar* obj = js->newObject("RTCP");
	BCVar* v = newSocketObj(cid, &in);
	BCNode* n = obj->getChildOrCreate("socket");
	n->replace(v);
	var->setReturnVar(obj);
}

void JSTCP::listen(KoalaJS* js, BCVar* var, void* data) {
	int backlog = var->getParameter("backlog")->getInt();
	var->getReturnVar()->setInt(0);

	SocketT* sc = getSocket(var);
	if(sc == NULL || sc->sid < 0)
		return;

	if(::listen(sc->sid, backlog) < 0) {
		return;
	}
	var->getReturnVar()->setInt(1);
}

void JSTCP::connect(KoalaJS* js, BCVar* var, void* data) {
	std::string host = var->getParameter("host")->getString();
	int port = var->getParameter("port")->getInt();
	int to = DEF_TIMEOUT;
	BCVar *n = var->getParameter("timeout");
	if(n != NULL)
		to = n->getInt();

	var->getReturnVar()->setInt(0);

	if(!DNSC::isIP(host.c_str()))
		host = DNSC::ip(host);

	if(host.length() == 0 || port <= 0)
		return;

	int sid = socket(PF_INET, SOCK_STREAM, 0);
	if(sid < 0)
		return;

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr)); 
	addr.sin_family = AF_INET; 
	addr.sin_addr.s_addr = inet_addr(host.c_str());
	addr.sin_port=htons(port);

	setSocket(var, sid, NULL);

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

