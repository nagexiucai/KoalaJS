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

  tinyJS.execute("var _terminated = 0; function exit() { _terminated = 1; } ");

  while(tinyJS.evaluate("_terminated") == "0") {
    char buffer[2048];
    fgets ( buffer, sizeof(buffer), stdin );

    try {
      tinyJS.execute(buffer);
    } catch (CScriptException *e) {
      printf("ERROR: %s\n", e->text.c_str());
    }
  }
}
