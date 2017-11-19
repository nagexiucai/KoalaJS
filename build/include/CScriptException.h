#ifndef BC_CSEXCEPTION_HH
#define BC_CSEXCEPTION_HH

#include <string>

class CScriptException {
	public:
		std::string text;
		CScriptException(const std::string &exceptionText);
};

#endif
