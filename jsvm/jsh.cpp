#include "Var.h"
#include "TinyJS.h"
#include "Compiler.h"
#include <stdio.h>
#include <string.h>

void _moduleLoader(CTinyJS* tinyJS);

void jshell() {
	std::string input;
	CTinyJS tinyJS;
	tinyJS.loadModule(_moduleLoader);
	printf("\033[4m\033[0;33m[Help] JSM Javascript engine, input javascript code, then input 'go;' to run, 'reset;' to reset or 'exit;' to quit.\033[0m\n\n");

	while(true) {
		printf("\033[4m\033[0;32mjsh:>\033[0m ");
		char buffer[2048+1];
		if(fgets ( buffer, 2048, stdin ) == NULL)
			break;

		if(strncmp(buffer, "exit;", 5) == 0) {
			printf("\n\033[4m\033[0;33m[Bye!]\033[0m\n");
			break;
		}
		else if(strncmp(buffer, "reset;", 6) == 0) {
			tinyJS.reset();
			input = "";
		}
		else if(strncmp(buffer, "go;", 3) == 0) {
			try {
				tinyJS.exec(input);
			} catch (CScriptException *e) {
				printf("ERROR: %s\n", e->text.c_str());
			}
			input = "";
		}
		else {
			input += buffer;
		}
	}	
}

int main(int argc, char** argv) {
	if(argc <= 1) {
		jshell();
	}
	else {
//while(true) {
		try {
			CTinyJS tinyJS;
			tinyJS.loadModule(_moduleLoader);
			
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
//}
	CodeCache::empty();
	return 0;
}
