#ifndef KOALA_JSON
#define KOALA_JSON

#include "CScriptLex.h"
#include "Var.h"

class JSONParser {
	static void block(CScriptLex* lex);
	static BCVar* funcDef(CScriptLex* lex);
	static BCVar* factor(CScriptLex* lex);
public: 
	static BCVar* parse(const string& str);
};

#endif
