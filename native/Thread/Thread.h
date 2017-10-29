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
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "exec(src, arg)", exec, js);
			addFunction(js, className, "run(file, arg)", run, js);
			addFunction(js, className, "sleep(sec)", sleep, NULL);
			addFunction(js, className, "usleep(usec)", usleep, NULL);
		}

		public:
		DECL_INSTANCE(JSThread)
	};

};

#endif
