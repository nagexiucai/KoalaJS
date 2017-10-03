#include "TinyJS/TinyJS.h"
#include "native/Debug/Debug.h"
#include "native/String/String.h"
#include "native/Array/Array.h"
#include "native/Math/Math.h"
#include "native/JSON/JSON.h"
#include "native/VM/VM.h"
#include "native/Base64/Base64.h"
#include "native/MD5/MD5.h"
#include "native/Thread/Thread.h"
#include "native/Global/Global.h"
#include "native/Bytes/Bytes.h"
#include "native/File/File.h"
#include "native/Socket/TCP.h"
#include "native/Socket/DNS.h"
#include "libs/File/File.h"

static void moduleLoader(CTinyJS* tinyJS) {
//load basic classes
	JSM::String::instance().load(tinyJS, "String");
	JSM::Array::instance().load(tinyJS, "Array");
	JSM::Bytes::instance().load(tinyJS, "Bytes");

//load Rkid extended classes
	JSM::Debug::instance().load(tinyJS, "Debug");

	JSM::Global::instance().load(tinyJS, "RGlobal");
	JSM::Math::instance().load(tinyJS, "RMath");
	JSM::JSON::instance().load(tinyJS, "RJSON");
	JSM::VM::instance().load(tinyJS, "RVM");
	JSM::JSBase64::instance().load(tinyJS, "RBase64");
	JSM::JSMD5::instance().load(tinyJS, "RMD5");
	JSM::JSThread::instance().load(tinyJS, "RThread");

	JSM::JSFile<JSM::JSFileNative>::instance().load(tinyJS, "RFile", NULL);
	JSM::JSTCP<JSM::JSTCPNative>::instance().load(tinyJS, "RTCP", NULL);
	JSM::JSDNS::instance().load(tinyJS, "RDNS");
}


int main(int argc, char** argv) {
	CTinyJS tinyJS;
	std::string input;

	tinyJS.loadModule(moduleLoader);

	printf("[Help] JSM Javascript engine, input javascript code, then input 'go!' to run or 'exit!' to quit.\n\n");

	if(argc <= 1) {
		while(true) {
			printf("JSM> ");

			char buffer[2048+1];
			if(fgets ( buffer, 2048, stdin ) == NULL)
				break;

			if(strncmp(buffer, "quit!", 5) == 0 ||
					strncmp(buffer, "exit!", 5) == 0) {
				break;
			}
			else if(strncmp(buffer, "go!", 3) == 0) {
				try {
					tinyJS.exec(input);
				} catch (CScriptException *e) {
					printf("ERROR: %s\n", e->text.c_str());
				}
				input = "";
			}
			else {
				input += buffer;
			}

		}	
	}
	else {
		try {
			tinyJS.run(argv[1]);
		} catch (CScriptException *e) {
			printf("ERROR: %s\n", e->text.c_str());
		}
	}
	return 0;
}
