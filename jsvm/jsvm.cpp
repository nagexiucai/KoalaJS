#include "Var.h"
#include "TinyJS.h"
#include "Compiler.h"
#include "../libs/File/File.h"
#include "../native/ClassLoader.h"
#include <string.h>

void _moduleLoader(CTinyJS* tinyJS);

int main(int argc, char** argv) {
	std::string input;

	if(argc <= 1) {
		printf("Usage: jsvm [.bcode file]\n");
		return 1;
	}
	else {
		try {
			string s = argv[1];
			TRACE("----Loading %s----\n", s.c_str());

			if(s.rfind(".bcode") != string::npos) { //run bytecode
				CTinyJS vm;
				vm.loadModule(_moduleLoader);
				vm.run(s);
			}
			else if(s.rfind(".js") != string::npos) { //run bytecode
				Compiler compiler;
				compiler.bytecode.reset();
				compiler.run(s);
				compiler.bytecode.dump();
				TRACE("---compiled to bytecode, continue run it---\n");

				CTinyJS vm;
				vm.loadModule(_moduleLoader);
				vm.exec(&compiler.bytecode);
			}		

			TRACE("----end----\n");
		} 
		catch (CScriptException *e) {
			TRACE("ERROR: %s\n", e->text.c_str());
		}
	}
	return 0;
}
