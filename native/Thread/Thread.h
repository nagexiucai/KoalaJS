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
		void registerFunctions(CTinyJS* tinyJS, const std::string& className);

		public:
		DECL_INSTANCE(JSThread)
	};

};

#endif
