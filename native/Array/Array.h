#ifndef JSM_ARRAY
#define JSM_ARRAY

#include "../ClassLoader.h"

namespace JSM {

	class Array : public JSClass {
		static void size(KoalaJS* js, CScriptVar *c,  void *);
		static void clear(KoalaJS* js, CScriptVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "size()", size, NULL);
			addFunction(js, className, "clear()", clear, NULL);
		}

		public:
		DECL_INSTANCE(Array)
	};

};

#endif
