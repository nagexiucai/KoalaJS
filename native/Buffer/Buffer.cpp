#include "Buffer.h"
#include "libs/File/File.h"
#include <cstdlib>

using namespace std;
using namespace JSM;

void Buffer::size(CScriptVar* var, void* data) {
	CScriptVar* v = var->getParameter("this");

	var->getReturnVar()->setInt(v->getInt());
}

void Buffer::from(CScriptVar* var, void* data) {
	std::string s = var->getParameter("src")->getString();
	CScriptVar* v = var->getParameter("this");

	unsigned char* buf = NULL;
	int size = s.length();

	if(size > 0) {
		buf = new unsigned char[size];	
		if(buf == NULL)
			size = 0;
		else
			memcpy(buf, s.c_str(), size);
	}

	v->setPoint(buf, size, NULL, true);	
//	var->setReturnVar(v);
}


void Buffer::registerFunctions(CTinyJS* tinyJS, const std::string& className) {
	addFunction(tinyJS, className, "size()", size, NULL);
	addFunction(tinyJS, className, "from(src)", from, NULL);
}

