#ifndef JSM_Timer
#define JSM_Timer

#include "../ClassLoader.h"

namespace JSM {

	class Timer : public JSClass {
		static void on(KoalaJS* js, BCVar *c, void *);

		static void off(KoalaJS* js, BCVar *c, void *);

		static void set(KoalaJS* js, BCVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "on()", on, NULL);
			addFunction(js, className, "set(usec)", set, NULL);
			addFunction(js, className, "off()", off, NULL);
		}

		public:
		DECL_INSTANCE(Timer)
	};

};

#endif
