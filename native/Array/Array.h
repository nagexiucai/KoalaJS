#ifndef JSM_ARRAY
#define JSM_ARRAY

#include "../ClassLoader.h"

namespace JSM {

	class Array : public JSClass {
		static void size(KoalaJS* js, BCVar *c,  void *);
		static void clear(KoalaJS* js, BCVar *c, void *);
		static void add(KoalaJS* js, BCVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "size()", size, NULL);
			addFunction(js, className, "add(v)", add, NULL);
			addFunction(js, className, "clear()", clear, NULL);
		}

		public:
		DECL_INSTANCE(Array)
	};

};

#endif
