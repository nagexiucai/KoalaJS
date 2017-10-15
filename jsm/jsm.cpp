#include "TinyJS/TinyJS.h"
#include "../native/ClassLoader.h"

void _moduleLoader(CTinyJS* tinyJS);

int main(int argc, char** argv) {
	CTinyJS tinyJS;
	std::string input;

	tinyJS.loadModule(_moduleLoader);

	if(argc <= 1) {
		printf("\033[4m\033[40;33m[Help] JSM Javascript engine, input javascript code, then input ':go' to run or ':quit' to quit.\033[0m\n\n");

		while(true) {
			printf("\033[4m\033[40;32mJSM>\033[0m ");

			char buffer[2048+1];
			if(fgets ( buffer, 2048, stdin ) == NULL)
				break;

			if(strncmp(buffer, ":quit", 2) == 0) {
				printf("\n\033[4m\033[40;33m[Bye!]\033[0m\n");
				break;
			}
			else if(strncmp(buffer, ":go", 2) == 0) {
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
	else {
		try {
			tinyJS.run(argv[1]);
		} catch (CScriptException *e) {
			printf("ERROR: %s\n", e->text.c_str());
		}
	}
	return 0;
}
