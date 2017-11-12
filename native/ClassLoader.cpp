#include "native/Debug/Debug.h"
#include "native/String/String.h"
#include "native/Math/Math.h"
#include "native/JSON/JSON.h"
#include "native/VM/VM.h"
#include "native/Base64/JSBase64.h"
#include "native/MD5/JSMD5.h"
#include "native/Thread/Thread.h"
#include "native/Global/Global.h"
#include "native/Bytes/Bytes.h"
#include "native/File/JSFile.h"
#include "native/Socket/TCP.h"
#include "native/Socket/DNS.h"
#include "native/System/System.h"
#include "native/DNS/JSDNSC.h"
#include "native/Dir/Dir.h"
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
	JSM::JSThread::instance().load(tinyJS, "RThread");
	JSM::JSBase64::instance().load(tinyJS, "RBase64");
	JSM::JSMD5::instance().load(tinyJS, "RMD5");
	JSM::JSFile::instance().load(tinyJS, "RFile");
	JSM::JSTCP::instance().load(tinyJS, "RTCP");
	JSM::JSDNSC::instance().load(tinyJS, "RDNSC");
	JSM::JSDir::instance().load(tinyJS, "RDir");
	JSM::System::instance().load(tinyJS, "RSystem");
	JSM::Time::instance().load(tinyJS, "RTime");
}
