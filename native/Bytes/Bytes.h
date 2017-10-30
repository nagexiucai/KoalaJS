#ifndef JSM_Bytes
#define JSM_Bytes

#include "../ClassLoader.h"

namespace JSM {

	class Bytes: public JSClass {
		static void size(KoalaJS* js, CScriptVar* var, void* data);

		static void from(KoalaJS* js, CScriptVar* var, void* data);

		static void toString(KoalaJS* js, CScriptVar* var, void* data);

		static void append(KoalaJS* js, CScriptVar* var, void* data);

		static void set(KoalaJS* js, CScriptVar* var, void* data);

		protected:
		inline void registerFunctions(KoalaJS* js, const std::string& className) {
			addFunction(js, className, "constructor(src)", from, NULL);
			addFunction(js, className, "size()", size, NULL);
			addFunction(js, className, "from(src)", from, NULL);
			addFunction(js, className, "append(src)", append, NULL);
			addFunction(js, className, "toString()", toString, NULL);
			addFunction(js, className, "set(at, ch)", set, NULL);
		}

		public:
		DECL_INSTANCE(Bytes)
	};

};

#endif
