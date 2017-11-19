#ifndef JS_CMD_HH
#define JS_CMD_HH

#include "KoalaJS.h"
#include "Compiler.h"
#include <stdio.h>

class JSCmd {
	//run js file.
	static void run(int argc, char** argv, JSModuleLoader moduleLoader, bool debug) {
		try {
			KoalaJS tinyJS;
			tinyJS.loadModule(moduleLoader);

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
public:
	static void cmd(int argc, char** argv, JSModuleLoader moduleLoader) {
		if(argc >= 2) {
			if(strcmp(argv[1], "-d") == 0) {
				run(argc, argv, moduleLoader, true);
			}
			else {
				run(argc, argv, moduleLoader, false);
			}
		}
		CodeCache::empty();
	}
};

#endif
