#include "DNS.h"
#include "libs/DNS/dnsc.h"

using namespace std;
using namespace JSM;

void JSDNS::ip(CScriptVar* var, void* data) {
	std::string host = var->getParameter("host")->getString();
	var->getReturnVar()->setString(DNSC::ip(host));
}

