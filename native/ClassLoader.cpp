#include "native/Debug/Debug.h"
#include "native/String/String.h"
#include "native/Math/Math.h"
#include "native/JSON/JSON.h"
#include "native/VM/VM.h"
#include "native/Global/Global.h"
#include "native/Bytes/Bytes.h"
#include "native/Array/Array.h"
#include "native/Thread/Thread.h"
#include "native/Time/Time.h"
#include "native/Timer/Timer.h"

extern "C" void _basicModuleLoader(KoalaJS* js) {
//load basic classes
	JSM::Bytes::instance().load(js, "Bytes");
	JSM::String::instance().load(js, "String");
	JSM::Array::instance().load(js, "Array");

//load Rkid extended classes
	JSM::Debug::instance().load(js, "Debug");

	JSM::Global::instance().load(js, "RGlobal");
	JSM::Math::instance().load(js, "RMath");
	JSM::JSON::instance().load(js, "RJSON");
	JSM::VM::instance().load(js, "RVM");
	JSM::Time::instance().load(js, "RTime");
	JSM::Timer::instance().load(js, "RTimer");
	JSM::JSThread::instance().load(js, "RThread");
}
