#include "native/Debug/Debug.h"
#include "native/String/String.h"
#include "native/Math/Math.h"
#include "native/JSON/JSON.h"
#include "native/VM/VM.h"
#include "native/Global/Global.h"
#include "native/Bytes/Bytes.h"
#include "native/Array/Array.h"
#include "native/Time/Time.h"

extern "C" void _basicModuleLoader(KoalaJS* tinyJS) {
//load basic classes
	JSM::Bytes::instance().load(tinyJS, "Bytes");
	JSM::String::instance().load(tinyJS, "String");
	JSM::Array::instance().load(tinyJS, "Array");

//load Rkid extended classes
	JSM::Debug::instance().load(tinyJS, "Debug");

	JSM::Global::instance().load(tinyJS, "RGlobal");
	JSM::Math::instance().load(tinyJS, "RMath");
	JSM::JSON::instance().load(tinyJS, "RJSON");
	JSM::VM::instance().load(tinyJS, "RVM");
	JSM::Time::instance().load(tinyJS, "RTime");
}
