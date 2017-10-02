#include "DNS.h"
#include "libs/DNS/DNS.h"
#include <arpa/inet.h> 
#include <netdb.h> 

using namespace std;
using namespace JSM;

void JSDNS::ip(CScriptVar* var, void* data) {
	std::string host = var->getParameter("host")->getString();
	var->getReturnVar()->setString(DNS::ip(host));
}

