#ifndef JSM_Global
#define JSM_Global

#include "../ClassLoader.h"
#include "utils/Thread/Thread.h"

typedef struct _BasicValue {
	const static unsigned char INT = 0;
	const static unsigned char DOUBLE = 1;
	const static unsigned char STRING = 2;
	const static unsigned char BYTES = 3;
	const static unsigned char UNDEF = 4;

	unsigned char type;
	struct V {
		int intV;
		double doubleV;
		std::string stringV;
		unsigned char* bytesV;

		inline V() {
			intV = 0;
			doubleV = 0;
			bytesV = NULL;
		}
		inline ~V() {
			if(bytesV != NULL)
				delete []bytesV;
		}
	}v;
} BasicValueT;

namespace JSM {

	class Global : public JSClass {

		static void set(KoalaJS* js, CScriptVar *c, void *);

		static void get(KoalaJS* js, CScriptVar *c, void *);

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
