#include "Compiler.h"
#include "Var.h"
#include "VM.h"
#include "libs/File/File.h"
#include <string.h>

void print(BCVar* v, void* data) {
	BCNode* n = v->getChild("txt");
	if(n != NULL)
		printf("%s", n->var->getString().c_str());
}

int main(int argc, char** argv) {
	Compiler compiler;
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
				compiler.exec(input);
				compiler.bytecode.dump();
				compiler.bytecode.toFile("out.bcode");
				break;
			}
			else if(strncmp(buffer, ":go", 2) == 0) {
				try {
					compiler.exec(input);
					compiler.bytecode.dump();
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
			
			if(s.rfind(".bcode") != string::npos) { //run bytecode
				while(true) {
				TRACE("----Loading %s----\n", s.c_str());
				VM vm;
				vm.registerNative("Debug", "print(txt)", print);
				vm.run(s);
				TRACE("----end----\n");
				}	
			}
			else { //compile js to bytecode.
				TRACE("Compiling : %s\n", s.c_str());
				compiler.bytecode.reset();
				compiler.run(argv[1]);
				s += ".bcode";

				int i = s.rfind("/");
				if(i != string::npos)
					s = s.substr(i+1);
				compiler.bytecode.toFile(s);
				compiler.bytecode.dump();
			}
		} catch (CScriptException *e) {
			TRACE("ERROR: %s\n", e->text.c_str());
		}
	}
	return 0;
}
