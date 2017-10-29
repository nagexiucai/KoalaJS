#ifndef JSM_Thread
#define JSM_Thread

#include "../ClassLoader.h"

namespace JSM {

	class JSThread : public JSClass {
		static void exec(CScriptVar *c, void *);

		static void run(CScriptVar *c, void *);

		static void sleep(CScriptVar *c, void *);

		static void usleep(CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "exec(src, arg)", exec, tinyJS);
			addFunction(tinyJS, className, "run(file, arg)", run, tinyJS);
			addFunction(tinyJS, className, "sleep(sec)", sleep, NULL);
			addFunction(tinyJS, className, "usleep(usec)", usleep, NULL);
		}

		public:
		DECL_INSTANCE(JSThread)
	};

};

#endif
