#ifndef JSM_Buffer
#define JSM_Buffer

#include "../ClassLoader.h"

namespace JSM {

	class Buffer: public JSClass {
		static void size(CScriptVar* var, void* data);

		static void from(CScriptVar* var, void* data);

		protected:
  	void registerFunctions(CTinyJS* tinyJS, const std::string& className);

		public:
		DECL_INSTANCE(Buffer)
	};

};

#endif
