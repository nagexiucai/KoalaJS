#ifndef JSM_JSDNS
#define JSM_JSDNS

#include "../ClassLoader.h"

namespace JSM {

class JSDNS: public JSClass {
	static void ip(CScriptVar* var, void* data);

	protected:
	inline void registerFunctions(KoalaJS* js, const std::string& className) {
		JSClass::addFunction(js, className, "ip(host)", ip, NULL);
	}

	public:
	DECL_INSTANCE(JSDNS)
};

};

#endif
