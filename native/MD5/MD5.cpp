#include "MD5.h"
#include "libs/MD5/md5.h"

using namespace std;
using namespace JSM;

std::string JSMD5::hashString(const unsigned char* s, unsigned int size) {
	char digest[16];

	md5_state_t state;

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)s, size);
	md5_finish(&state, (md5_byte_t *)digest);

	std::string ret;
	ret.resize(16);
	std::copy(digest,digest+16,ret.begin());

	return ret;
}

const char _hexval[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string JSMD5::hashHex(const unsigned char* s, unsigned int size) {
	std::string hash = JSMD5::hashString(s, size);
	std::string hex;

	for (size_t i = 0; i < hash.size(); i++) {
		hex.push_back(_hexval[((hash[i] >> 4) & 0xF)]);
		hex.push_back(_hexval[(hash[i]) & 0x0F]);
	}

	return hex;
}

void JSMD5::hashString(CScriptVar *c, void *userdata) {
	std::string src = c->getParameter("src")->getString();

	std::string ret = hashString((const unsigned char*)src.c_str(), src.length());
	c->getReturnVar()->setString(ret);
}

void JSMD5::hashHex(CScriptVar *c, void *userdata) {

	std::string src = c->getParameter("src")->getString();
	std::string ret = hashHex((const unsigned char*)src.c_str(), src.length());

	c->getReturnVar()->setString(ret);
}

void JSMD5::registerFunctions(CTinyJS* tinyJS, const std::string& className) {
	addFunction(tinyJS, className, "hashString(src)", hashString, NULL);
	addFunction(tinyJS, className, "hashHex(src)", hashHex, NULL);
}

