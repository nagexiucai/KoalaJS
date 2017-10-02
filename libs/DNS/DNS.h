#ifndef DNS_HH
#define DNS_HH

#include <arpa/inet.h> 
#include <netdb.h> 
#include <string> 

class DNS {
public:
	static std::string ip(std::string hname) {
		struct hostent *h;
		if((h = gethostbyname(hname.c_str())) == NULL) {
			return "";
		}

		return inet_ntoa(*((struct in_addr *)h->h_addr));
	}
};

#endif

