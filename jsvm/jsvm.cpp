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
		printf("Usage: jsvm [.bcode or .js file] {args....}\n");
		return 1;
	}
	else {
//while(true) {
		try {
			CTinyJS vm;
			vm.loadModule(_moduleLoader);
			
			//read args 
			BCVar* args = new BCVar();
			args->setArray();
			for(int i=2; i<argc; ++i) {
				args->setArrayIndex(i-2, new BCVar(argv[i]));
			}
			vm.getRoot()->addChild("_args", args);

			vm.run(argv[1]);
		} 
		catch (CScriptException *e) {
			TRACE("ERROR: %s\n", e->text.c_str());
		}
	}
//}
	CodeCache::empty();
	return 0;
}
