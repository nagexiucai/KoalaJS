#include "Compiler.h"
#include "Var.h"
#include "libs/File/File.h"
#include <string.h>

int main(int argc, char** argv) {
	Compiler tinyJS;
	std::string input;

	if(argc <= 1) {

		while(true) {

			char buffer[2048+1];
			if(fgets ( buffer, 2048, stdin ) == NULL)
				break;

			if(strncmp(buffer, ":quit", 2) == 0) {
				break;
			}
			else if(strncmp(buffer, ":c", 2) == 0) {
				tinyJS.exec(input);
				tinyJS.bytecode.dump();
				tinyJS.bytecode.toFile("out.bcode");
				break;
			}
			else if(strncmp(buffer, ":go", 2) == 0) {
				try {
					tinyJS.exec(input);
					tinyJS.bytecode.dump();
				} catch (CScriptException *e) {
					TRACE("ERROR: %s\n", e->text.c_str());
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
			string s = argv[1];
			if(s.rfind(".bcode") != string::npos) {
				TRACE("Loading : %s\n", s.c_str());
				tinyJS.bytecode.fromFile(s);
			}
			else {
				TRACE("Compiling : %s\n", s.c_str());
				tinyJS.run(argv[1]);
				s += ".bcode";

				int i = s.rfind("/");
				if(i != string::npos)
					s = s.substr(i+1);

				tinyJS.bytecode.toFile(s);
			}

			tinyJS.bytecode.dump();
		} catch (CScriptException *e) {
			TRACE("ERROR: %s\n", e->text.c_str());
		}
	}
	return 0;
}
