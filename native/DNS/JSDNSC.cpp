#include "JSDNSC.h"
#include "utils/DNS/dnsc.h"

using namespace std;
using namespace JSM;

void JSDNSC::resolv(KoalaJS* js, BCVar *c, void *userdata) {
	std::string domain = c->getParameter("domain")->getString();
	std::string type = c->getParameter("type")->getString();
	
	int t = DNS_A_RECORD;
	
	if(type == "MX") {
		t = DNS_MX_RECORD;
	}
	else if(type == "CNAME") {
		t = DNS_CNAME_RECORD;
	}
	else if(type == "NS") {
		t = DNS_NS_RECORD;
	}
	
	vector<DNSRes> res;
	DNSC::resolv(domain.c_str(), t, res);

	BCVar *ret = c->getReturnVar();
	ret->setArray();
	size_t sz = res.size();
	for(int i=0; i<sz; ++i) {
		ret->setArrayIndex(i, new BCVar(res[i].res));
	}
}

