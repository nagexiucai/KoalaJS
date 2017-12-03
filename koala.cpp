#include "JSCmd.h"

extern "C" void _basicModuleLoader(KoalaJS* tinyJS);

//run js file.
void compile(const string& fname, bool tofile = false, bool debug = false) {
	try {
		Compiler compiler;
		Bytecode bytecode;
		if(fname.find(".bcode") != string::npos) {
			bytecode.fromFile(fname);
			if(!tofile) {
				string s = bytecode.dump();
				TRACE("%s\n", s.c_str());
			}
		}
		else {
			compiler.run(fname, &bytecode, debug);
			if(tofile)
				bytecode.toFile(fname + ".bcode");
			else {
				string s = bytecode.dump();
				TRACE("%s\n", s.c_str());
			}
		}
	} 
	catch (CScriptException *e) {
		ERR("ERROR: %s\n", e->text.c_str());
	}
}

int main(int argc, char** argv) {
	if(argc <= 1) {
		ERR("Usage: %s [-v] [-c] [filename.js/.bcode]\n", argv[0]);
		return 1;
	}
	else if(argc >= 2) {
		if(strcmp(argv[1], "-v") == 0) {
			compile(argv[2], false, true);
		}
		else if(strcmp(argv[1], "-c") == 0) {
			compile(argv[2], true);
		}
		else
			JSCmd::cmd(argc, argv, _basicModuleLoader);
	}
	return 0;
}
