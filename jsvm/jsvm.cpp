#include "Var.h"
#include "TinyJS.h"
#include "../libs/File/File.h"
#include "../native/ClassLoader.h"
#include <string.h>

#include "native/Debug/Debug.h"
#include "native/Math/Math.h"
#include "native/Bytes/Bytes.h"
/*
#include "native/String/String.h"
#include "native/Array/Array.h"
#include "native/JSON/JSON.h"
#include "native/VM/VM.h"
#include "native/Base64/Base64.h"
#include "native/MD5/MD5.h"
#include "native/Thread/Thread.h"
#include "native/Global/Global.h"
#include "native/File/File.h"
#include "native/Socket/TCP.h"
#include "native/Socket/DNS.h"
#include "native/System/System.h"
*/

static void moduleLoader(CTinyJS* tinyJS) {
	JSM::Debug::instance().load(tinyJS, "Debug");

//load basic classes
	JSM::Bytes::instance().load(tinyJS, "Bytes");
//	JSM::String::instance().load(tinyJS, "String");
//	JSM::Array::instance().load(tinyJS, "Array");

/*
//load Rkid extended classes
	JSM::Global::instance().load(tinyJS, "RGlobal");
	JSM::JSON::instance().load(tinyJS, "RJSON");
	JSM::VM::instance().load(tinyJS, "RVM");
	JSM::JSBase64::instance().load(tinyJS, "RBase64");
	JSM::JSMD5::instance().load(tinyJS, "RMD5");
	JSM::JSThread::instance().load(tinyJS, "RThread");
	JSM::JSFile<JSM::JSFileNative>::instance().load(tinyJS, "RFile");
	JSM::JSTCP<JSM::JSTCPNative>::instance().load(tinyJS, "RTCP");
	JSM::JSDNS::instance().load(tinyJS, "RDNS");
	JSM::System::instance().load(tinyJS, "RSystem");
*/
//	JSM::Math::instance().load(tinyJS, "RMath");
}

int main(int argc, char** argv) {
	std::string input;

	if(argc <= 1) {
		printf("Usage: jsvm [.bcode file]\n");
		return 1;
	}
	else {
		try {
			string s = argv[1];
			
			if(s.rfind(".bcode") != string::npos) { //run bytecode
				while(true) {
				TRACE("----Loading %s----\n", s.c_str());
				CTinyJS vm;
				vm.loadModule(moduleLoader);
				vm.run(s);
				TRACE("----end----\n");
				}	
			}
		} 
		catch (CScriptException *e) {
			TRACE("ERROR: %s\n", e->text.c_str());
		}
	}
	return 0;
}
