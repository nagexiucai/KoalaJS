#include "Var.h"
#include "TinyJS.h"
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
			
			if(s.rfind(".bcode") != string::npos) { //run bytecode
//				while(true) {
				TRACE("----Loading %s----\n", s.c_str());
				CTinyJS vm;
				vm.loadModule(_moduleLoader);
				vm.run(s);
				TRACE("----end----\n");
//				}	
			}
		} 
		catch (CScriptException *e) {
			TRACE("ERROR: %s\n", e->text.c_str());
		}
	}
	return 0;
}
