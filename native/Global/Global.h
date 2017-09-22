#ifndef JSM_Global
#define JSM_Global

#include "../ClassLoader.h"
#include "libs/Thread/Thread.h"

namespace JSM {

	class Global : public JSClass {

		static void set(CScriptVar *c, void *);

		static void get(CScriptVar *c, void *);

		protected:
		void registerFunctions(CTinyJS* tinyJS, const std::string& className);

		public:
		DECL_INSTANCE(Global)
	};

};

#endif
