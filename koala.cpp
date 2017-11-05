#include "Var.h"
#include "KoalaJS.h"
#include "Compiler.h"
#include <stdio.h>
#include <string.h>

extern "C" void _basicModuleLoader(KoalaJS* tinyJS);

//run js file.
void run(int argc, char** argv) {
	try {
		KoalaJS tinyJS;
		tinyJS.loadModule(_basicModuleLoader);

		//read args 
		BCVar* args = new BCVar();
		args->setArray();
		for(int i=2; i<argc; ++i) {
			args->setArrayIndex(i-2, new BCVar(argv[i]));
		}
		tinyJS.getRoot()->addChild("_args", args);
		tinyJS.run(argv[1]);
	} 
	catch (CScriptException *e) {
		ERR("ERROR: %s\n", e->text.c_str());
	}
}

//run js file.
void compile(const string& fname, bool tofile = false) {
	try {
		Compiler compiler;
		if(fname.find(".bcode") != string::npos) {
			compiler.bytecode.fromFile(fname);
		}
		else {
			compiler.run(fname);
			if(tofile)
				compiler.bytecode.toFile(fname + ".bcode");
			else {
				string s = compiler.bytecode.dump();
				printf("%s\n", s.c_str());
			}
		}
	} 
	catch (CScriptException *e) {
		ERR("ERROR: %s\n", e->text.c_str());
	}
}

int main(int argc, char** argv) {
	if(argc <= 1) {
		ERR("Usage: jsh [-v] [-c] [filename.js/.bcode]\n");
		return 1;
	}
	else if(argc >= 2) {
		if(strcmp(argv[1], "-v") == 0) {
			compile(argv[2]);
		}
		else if(strcmp(argv[1], "-c") == 0) {
			compile(argv[2], true);
		}
		else {
		//while(true) {
		run(argc, argv);
		//}	
		}
	}
	CodeCache::empty();
	return 0;
}
