#include "Compiler.h"
#include "Var.h"
#include "utils/File/File.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char** argv) {
	Compiler compiler;
	std::string input;

	if(argc <= 1) {
		TRACE("Usage: bc [file.js] (-t to test-only)\n");
		return 1;
	}
	else {
		try {
			string s = argv[1];
			TRACE("Compiling : %s\n", s.c_str());
			compiler.bytecode.reset();
			compiler.run(argv[1]);

			if(argc > 2) {
				s = argv[2];
				if(s == "-t") {
					s = compiler.bytecode.dump();
					TRACE("%s\n", s.c_str());
				}
			}
			else {
				s += ".bcode";
				int i = s.rfind("/");
				if(i != string::npos)
					s = s.substr(i+1);
				compiler.bytecode.toFile(s);
			}
		} catch (CScriptException *e) {
			TRACE("ERROR: %s\n", e->text.c_str());
		}
	}
	return 0;
}
