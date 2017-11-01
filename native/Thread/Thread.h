#ifndef JSM_Thread
#define JSM_Thread

#include "../ClassLoader.h"

namespace JSM {

	class JSThread : public JSClass {
		static void exec(KoalaJS* js, BCVar *c, void *);

		static void run(KoalaJS* js, BCVar *c, void *);

		static void sleep(KoalaJS* js, BCVar *c, void *);

		static void usleep(KoalaJS* js, BCVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "exec(src, arg)", exec, NULL);
			addFunction(js, className, "run(file, arg)", run, NULL);
			addFunction(js, className, "sleep(sec)", sleep, NULL);
			addFunction(js, className, "usleep(usec)", usleep, NULL);
		}

		public:
		DECL_INSTANCE(JSThread)
	};

};

#endif
