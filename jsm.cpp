#include "TinyJS/TinyJS.h"
#include "native/Debug/Debug.h"
#include "native/String/String.h"
#include "native/Array/Array.h"
#include "native/Math/Math.h"

int main(int argc, char** argv) {
  CTinyJS tinyJS;

  JSM::Debug::instance().load(&tinyJS, "Debug");
  JSM::String::instance().load(&tinyJS, "String");
  JSM::Array::instance().load(&tinyJS, "Array");
  JSM::Math::instance().load(&tinyJS, "Math");

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
