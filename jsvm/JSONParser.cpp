#include "JSONParser.h"
#include <stdlib.h>

void JSONParser::block(CScriptLex* lex) { //TODO: statements parsing
	lex->chkread('{');
	while (lex->tk!='}') {
		if(lex->tk == '{')
			block(lex);
		else
			lex->chkread(lex->tk);
	}
	lex->chkread('}');
}

BCVar* JSONParser::funcDef(CScriptLex* lex) {
	BCVar* ret = new BCVar();
	ret->type = BCVar::FUNC;
	//do arguments
	vector<string> args;

	lex->chkread('(');
	while (lex->tk!=')') {
		args.push_back(lex->tkStr);
		lex->chkread(LEX_ID);
		if (lex->tk!=')') lex->chkread(',');
	}
	lex->chkread(')');
	//function  body
	block(lex);

	ret->setFunction(0);
	ret->getFunc()->args = args;
	return ret;
}

BCVar* JSONParser::factor(CScriptLex* lex) {
	if (lex->tk==LEX_R_TRUE) {
		lex->chkread(LEX_R_TRUE);
		return new BCVar(1);
	}
	else if (lex->tk==LEX_R_FALSE) {
		lex->chkread(LEX_R_FALSE);
		return new BCVar(0);
	}
	else if (lex->tk==LEX_R_NULL) {
		lex->chkread(LEX_R_NULL);
		return new BCVar();
	}
	else if (lex->tk==LEX_R_UNDEFINED) {
		lex->chkread(LEX_R_UNDEFINED);
		return new BCVar();
	}
	else if (lex->tk==LEX_INT) {
		int i = atoi(lex->tkStr.c_str());
		lex->chkread(LEX_INT);
		return new BCVar(i);
	}
	else if (lex->tk==LEX_FLOAT) {
		float f = atof(lex->tkStr.c_str());
		lex->chkread(LEX_FLOAT);
		return new BCVar(f);
	}
	else if (lex->tk==LEX_STR) {
		string s = lex->tkStr;
		lex->chkread(LEX_STR);
		return new BCVar(s);
	}
	else if(lex->tk==LEX_R_FUNCTION) {
		lex->chkread(LEX_R_FUNCTION);
		return funcDef(lex);
	}
	else if (lex->tk=='[') {
		/* JSON-style array */
		BCVar* arr = new BCVar();
		arr->type = BCVar::ARRAY;
		lex->chkread('[');
		while (lex->tk != ']') {
			BCVar* v = factor(lex);
			arr->addChild("", v);
			if (lex->tk != ']') lex->chkread(',');
		}
		lex->chkread(']');
		return arr;
	}
	else if (lex->tk=='{') {
		lex->chkread('{');
		BCVar* obj = new BCVar();
		obj->type = BCVar::OBJECT;
		while(lex->tk != '}') {
			string id = lex->tkStr;
			if(lex->tk == LEX_STR)
				lex->chkread(LEX_STR);
			else
				lex->chkread(LEX_ID);

			lex->chkread(':');
			BCVar* v = factor(lex);

			obj->addChild(id, v);

			if(lex->tk != '}')
				lex->chkread(',');
		}
		lex->chkread('}');
		return obj;
	}
	return new BCVar();
}

BCVar* JSONParser::parse(const string& str) {
	CScriptLex lex(str);
	BCVar* ret = factor(&lex);
	return ret;
}
