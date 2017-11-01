#ifndef JSM_Global
#define JSM_Global

#include "../ClassLoader.h"

namespace JSM {

	class Global : public JSClass {

		static void set(KoalaJS* js, BCVar *c, void *);

		static void get(KoalaJS* js, BCVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "get(name)", get, NULL);
			addFunction(js, className, "set(name, value)", set, NULL);
		}

		public:
		DECL_INSTANCE(Global)
	};

};

#endif
