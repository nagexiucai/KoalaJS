#include "Var.h"
#include "TinyJS.h"
#include "Compiler.h"
#include "utils/String/StringUtil.h"
#include <stdio.h>
#include <string.h>

void _moduleLoader(CTinyJS* tinyJS);

void help() {
	printf("\033[4m\033[0;33m[Help] JSM Javascript engine, you can run javascript file directly like \"jsh [file]\", or input javascript code here.\n"
			"commands:\n"
			"\thelp       : show this help.\n"
			"\tbc         : compiler the whole javascript to bytecode.\n"
			"\tbc  [file] : compiler file to bytecode. will reset runtime.\n"
			"\treset      : reset runtime, clear all variable and stack.\n"
			"\trun        : run the new input javascript.\n"
			"\trun [file] : run file, will reset runtime first.\n"
			"\texit       : quit shell.\033[0m\n");
}

void jshell() {
	std::string input;
	std::string run;
	CTinyJS tinyJS;
	tinyJS.loadModule(_moduleLoader);
	help();

	while(true) {
		printf("\033[4m\033[0;32mjsh:>\033[0m ");
		char buffer[2048+1];
		if(fgets ( buffer, 2048, stdin ) == NULL)
			break;

		string ln = buffer;
		ln = StringUtil::trim(ln);

		if(ln == "help") {
			help();
		}
		else if(ln == "exit") {
			printf("\n\033[4m\033[0;33m[Bye!]\033[0m\n");
			break;
		}
		else if(ln == "reset") {
			tinyJS.reset();
			input = "";
			run = "";
		}
		else if(ln == "run") {
			try {
				tinyJS.exec(run);
			} catch (CScriptException *e) {
				printf("ERROR: %s\n", e->text.c_str());
			}
			run = "";
		}
		else if(ln.find("run ") != string::npos) {
			try {
				tinyJS.reset();
				tinyJS.run(ln.substr(4));
			} catch (CScriptException *e) {
				printf("ERROR: %s\n", e->text.c_str());
			}
			run = "";
			input = "";
		}
		else if(ln == "bc") {
			Compiler compiler;
			try {
				compiler.exec(input);
				string s = compiler.bytecode.dump();
				printf("%s\n", s.c_str());
			} catch (CScriptException *e) {
				printf("ERROR: %s\n", e->text.c_str());
			}
		}
		else if(ln.find("bc ") != string::npos) {
			Compiler compiler;
			try {
				compiler.run(ln.substr(3));
				string s = compiler.bytecode.dump();
				printf("%s\n", s.c_str());
			} catch (CScriptException *e) {
				printf("ERROR: %s\n", e->text.c_str());
			}
			run = "";
			input = "";
		}
		else {
			run += ln;
			input += ln;
		}
	}	
}

void run(int argc, char** argv) {
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

int main(int argc, char** argv) {
	if(argc <= 1) {
		jshell();
	}
	else {
		//while(true) {
		run(argc, argv);
		//}
	}
	CodeCache::empty();
	return 0;
}
