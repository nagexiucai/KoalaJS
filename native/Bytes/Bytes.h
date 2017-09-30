#ifndef JSM_Bytes
#define JSM_Bytes

#include "../ClassLoader.h"

namespace JSM {

	class Bytes: public JSClass {
		static void size(CScriptVar* var, void* data);

		static void from(CScriptVar* var, void* data);

		static void toString(CScriptVar* var, void* data);

		static void append(CScriptVar* var, void* data);

		protected:
  	void registerFunctions(CTinyJS* tinyJS, const std::string& className);

		public:
		DECL_INSTANCE(Bytes)
	};

};

#endif
