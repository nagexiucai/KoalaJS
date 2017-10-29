#ifndef JSM_Bytes
#define JSM_Bytes

#include "../ClassLoader.h"

namespace JSM {

	class Bytes: public JSClass {
		static void size(CScriptVar* var, void* data);

		static void from(CScriptVar* var, void* data);

		static void toString(CScriptVar* var, void* data);

		static void append(CScriptVar* var, void* data);

		static void set(CScriptVar* var, void* data);

		protected:
		inline void registerFunctions(KoalaJS* tinyJS, const std::string& className) {
			addFunction(tinyJS, className, "constructor(src)", from, NULL);
			addFunction(tinyJS, className, "size()", size, NULL);
			addFunction(tinyJS, className, "from(src)", from, NULL);
			addFunction(tinyJS, className, "append(src)", append, NULL);
			addFunction(tinyJS, className, "toString()", toString, NULL);
			addFunction(tinyJS, className, "set(at, ch)", set, NULL);
		}

		public:
		DECL_INSTANCE(Bytes)
	};

};

#endif
