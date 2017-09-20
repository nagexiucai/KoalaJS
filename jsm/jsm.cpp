#include "TinyJS/TinyJS.h"
#include "native/Debug/Debug.h"
#include "native/String/String.h"
#include "native/Array/Array.h"
#include "native/Math/Math.h"
#include "native/JSON/JSON.h"
#include "native/VM/VM.h"
#include "native/Base64/Base64.h"
#include "native/MD5/MD5.h"

int main(int argc, char** argv) {
	CTinyJS tinyJS;

	JSM::Debug::instance().load(&tinyJS, "Debug");
	JSM::String::instance().load(&tinyJS, "String");
	JSM::Array::instance().load(&tinyJS, "Array");
	JSM::Math::instance().load(&tinyJS, "Math");
	JSM::JSON::instance().load(&tinyJS, "JSON");
	JSM::VM::instance().load(&tinyJS, "VM");
	JSM::JSBase64::instance().load(&tinyJS, "Base64");
	JSM::JSMD5::instance().load(&tinyJS, "MD5");

	std::string input;

	while(true) {
		char buffer[2048+1];
		if(fgets ( buffer, 2048, stdin ) == NULL)
			break;
		input += buffer;
	}	

	try {
		tinyJS.execute(input);
	} catch (CScriptException *e) {
		printf("ERROR: %s\n", e->text.c_str());
	}

	return 0;
}
