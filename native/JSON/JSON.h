#ifndef JSM_JSON
#define JSM_JSON

#include "../ClassLoader.h"

namespace JSM {

	class JSON : public JSClass {
		static void stringify(CScriptVar *c, void *);

		static void parse(CScriptVar *c, void *);

		protected:
		void registerFunctions(CTinyJS* tinyJS, const std::string& className);

		public:
		DECL_INSTANCE(JSON)
	};

};

#endif
