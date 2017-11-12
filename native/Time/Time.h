#ifndef JSM_Time
#define JSM_Time

#include "../ClassLoader.h"

namespace JSM {

	class Time : public JSClass {
		static void timeOfDay(KoalaJS* js, BCVar *c, void *);

		static void time(KoalaJS* js, BCVar *c, void *);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "time()", time, NULL);
			addFunction(js, className, "timeOfDay()", timeOfDay, NULL);
		}

		public:
		DECL_INSTANCE(Time)
	};

};

#endif
