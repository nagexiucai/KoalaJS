#include "DNS.h"
#include "utils/DNS/dnsc.h"

using namespace std;
using namespace JSM;

void JSDNS::ip(KoalaJS* js, CScriptVar* var, void* data) {
	std::string host = var->getParameter("host")->getString();
	var->getReturnVar()->setString(DNSC::ip(host));
}

