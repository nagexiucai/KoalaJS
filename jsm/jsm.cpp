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
#include "libs/File/File.h"

static void moduleLoader(CTinyJS* tinyJS) {
//load basic classes
	JSM::Debug::instance().load(tinyJS, "Debug");
	JSM::String::instance().load(tinyJS, "String");
	JSM::Array::instance().load(tinyJS, "Array");

//load Rokid extended classes
	JSM::Math::instance().load(tinyJS, "R.Math");
	JSM::JSON::instance().load(tinyJS, "R.JSON");
	JSM::VM::instance().load(tinyJS, "R.VM");
	JSM::JSBase64::instance().load(tinyJS, "R.Base64");
	JSM::JSMD5::instance().load(tinyJS, "R.MD5");
	JSM::JSThread::instance().load(tinyJS, "R.Thread");
	JSM::Global::instance().load(tinyJS, "R.Global");
}


int main(int argc, char** argv) {
	CTinyJS tinyJS;
	std::string input;

	tinyJS.loadModule(moduleLoader);

	if(argc <= 1) {
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
