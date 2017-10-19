#ifndef DNSC_HH
#define DNSC_HH

#include <string>
#include <vector>

#include "dns.h"

typedef struct {
	std::string res;
	int cacheTime;
} DNSRes;

#define DEFAULT_SERVER "8.8.8.8"

class DNSC {
public:
	static bool resolv(const char* server, const char* domain, int type, std::vector<DNSRes>& res);

	static bool resolv(const char* server, const char* domain, std::string& res);
	
	static std::string ip(const std::string& domain); 
};

#endif
