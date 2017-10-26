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
	static bool resolv(const char* domain, int type, std::vector<DNSRes>& res, const char* server = DEFAULT_SERVER);
	
	static bool isIP(const char* host); 

	static std::string ip(const std::string& domain); 
};

#endif
