#include "Var.h"
#include "KoalaJS.h"
#include "Compiler.h"
#include <stdio.h>
#include <string.h>

extern "C" void _basicModuleLoader(KoalaJS* tinyJS);

//run js file.
void run(int argc, char** argv, bool debug) {
	try {
		KoalaJS tinyJS;
		tinyJS.loadModule(_basicModuleLoader);

		//read args 
		BCVar* args = new BCVar();
		args->setArray();

		int i=2;
		if(debug)
			i++;
		argc -= i;
		for(int j=0; j<argc; ++j) {
			args->setArrayIndex(j, new BCVar(argv[i+j]));
		}
		tinyJS.getRoot()->addChild("_args", args);
		tinyJS.run(argv[i-1], debug);
	} 
	catch (CScriptException *e) {
		ERR("ERROR: %s\n", e->text.c_str());
	}
}

//run js file.
void compile(const string& fname, bool tofile = false, bool debug = false) {
	try {
		Compiler compiler;
		Bytecode bytecode;
		if(fname.find(".bcode") != string::npos) {
			bytecode.fromFile(fname);
			if(!tofile) {
				string s = bytecode.dump();
				printf("%s\n", s.c_str());
			}
		}
		else {
			compiler.run(fname, &bytecode, debug);
			if(tofile)
				bytecode.toFile(fname + ".bcode");
			else {
				string s = bytecode.dump();
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
		ERR("Usage: koala [-v] [-c] [-d] [filename.js/.bcode]\n");
		return 1;
	}
	else if(argc >= 2) {
		if(strcmp(argv[1], "-v") == 0) {
			compile(argv[2], false, true);
		}
		else if(strcmp(argv[1], "-c") == 0) {
			compile(argv[2], true);
		}
		else if(strcmp(argv[1], "-d") == 0) {
			run(argc, argv, true);
		}
		else {
		//while(true) {
			run(argc, argv, false);
		//}	
		}
	}
	CodeCache::empty();
	return 0;
}
