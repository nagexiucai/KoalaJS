#include "CScriptLex.h"
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static unsigned char cmap[256]={
	//+0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
	0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,//00
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//10
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//20
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0,//30
	0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,//40
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 4,//50
	0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,//60
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0,//70
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//80
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//90
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//A0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//B0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//C0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//D0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//E0
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//F0
}; 
using namespace std;

#ifdef __GNUC__
#define vsprintf_s vsnprintf
#define sprintf_s snprintf
#define _strdup strdup
#endif


// ----------------------------------------------------------------------------------- Utils
bool isWhitespace(unsigned char ch) {
	return (cmap[ch]&1);//(ch==' ') || (ch=='\t') || (ch=='\n') || (ch=='\r');
}
////////////////////////////////////////////////////////////////////////////////
bool isNumeric(unsigned char ch) {
	return (cmap[ch]&2);//(ch>='0') && (ch<='9');
}
////////////////////////////////////////////////////////////////////////////////
bool isNumber(const std::string &str) {
	for (size_t i=0;i<str.size();i++){
		if (!isNumeric(str[i])) return false;
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////
bool isHexadecimal(unsigned char ch) {
	return ((ch>='0') && (ch<='9')) ||
		((ch>='a') && (ch<='f')) ||
		((ch>='A') && (ch<='F'));
}
////////////////////////////////////////////////////////////////////////////////
bool isAlpha(unsigned char ch) {
	return (cmap[ch]&4);//((ch>='a') && (ch<='z')) || ((ch>='A') && (ch<='Z')) || ch=='_';
}
////////////////////////////////////////////////////////////////////////////////
//bool isIDString(const char *s) {
//    if (!isAlpha(*s)){
//        return false;
//    }
//    while (*s) {
//        if (!(isAlpha(*s) || isNumeric(*s))){
//            return false;
//        }
//        s++;
//    }
//    return true;
//}
////////////////////////////////////////////////////////////////////////////////
char* oneLine(char *s, int ptr,int end)
{
	size_t cnt=0;
	static char work[1024];
	if( end < ptr ){
		ptr = end;
	}
	if( ptr>0) ptr--;
	if( ptr>0) ptr--;
	while( ptr > 0  && s[ptr] != '\n'){
		ptr--;
	}
	//ptr++;
	while( s[ptr] && s[ptr] != '\n' && cnt < sizeof( work )-2 ){
		work[cnt] = s[ptr];
		cnt++;
		ptr++;
	}
	work[cnt] = 0;
	return work;
}
////////////////////////////////////////////////////////////////////////////////
/// convert the given std::string into a quoted std::string suitable for javascript
std::string getJSString(const std::string &str) {
	std::string nStr = str;
	for (size_t i=0;i<nStr.size();i++) {
		const char *replaceWith = "";
		bool replace = true;

		switch (nStr[i]) {
			case '\\': replaceWith = "\\\\"; break;
			case '\n': replaceWith = "\\n"; break;
			case '\r': replaceWith = "\\r"; break;
			case '\a': replaceWith = "\\a"; break;
			case '"':  replaceWith = "\\\""; break;
			default: {
								 int nCh = ((int)nStr[i]) &0xFF;
								 if (nCh<32 || nCh>127) {
									 char bytes[5];
									 snprintf(bytes, 5, "\\x%02X", nCh);
									 replaceWith = bytes;
								 } else {
									 replace=false;
								 }
							 }
		}

		if (replace) {
			nStr = nStr.substr(0, i) + replaceWith + nStr.substr(i+1);
			i += strlen(replaceWith)-1;
		}
	}
	return "\"" + nStr + "\"";
}

/** Is the std::string alphanumeric */
bool isAlphaNum(const std::string &str) {
	if (str.size()==0){
		return true;
	}
	if (!isAlpha(str[0])){
		return false;
	}
	for (size_t i=0;i<str.size();i++){
		if (!(isAlpha(str[i]) || isNumeric(str[i]))){
			return false;
		}
	}
	return true;
}

// ----------------------------------------------------------------------------------- CSCRIPTLEX
CScriptLex::CScriptLex(const std::string &input) {
	data = strdup(input.c_str());
	dataOwned = true;
	dataStart = 0;
	dataEnd = (int)strlen(data);
	reset();
}

CScriptLex::CScriptLex(CScriptLex *owner, int startChar, int endChar) {
	data = owner->data;
	dataOwned = false;
	dataStart = startChar;
	dataEnd = endChar;
	reset();
}

CScriptLex::~CScriptLex(void)
{
	if (dataOwned){
		free((void*)data);
	}
}

void CScriptLex::reset() {
	dataPos      = dataStart;
	tokenStart   = 0;
	tokenEnd     = 0;
	tokenLastEnd = 0;
	tk           = LEX_EOF;
	tkStr        = "";
	getNextCh();
	getNextCh();
	getNextToken();
}

void CScriptLex::chkread(int expected_tk) {
	if (tk!=expected_tk) {
		std::stringstream ss;
		ss << "Got " << 
			getTokenStr(tk).c_str() << "expected " << 
			getTokenStr(expected_tk).c_str() << ".";
			//getTokenStr(expected_tk).c_str() << " at " <<
			//getPosition(tokenStart).c_str() << "(" << oneLine(data,dataPos,dataEnd) << ")\n";
		throw new CScriptException(ss.str());
	}
	getNextToken();
}

std::string CScriptLex::getTokenStr(int token) {
	if (token>32 && token<128) {
		char buf[4] = "' '";
		buf[1] = (char)token;
		return std::string(buf);
	}
	switch (token) {
		case LEX_EOF            : return "EOF";
		case LEX_ID             : return "ID";
		case LEX_INT            : return "INT";
		case LEX_FLOAT          : return "FLOAT";
		case LEX_STR            : return "STRING";
		case LEX_EQUAL          : return "==";
		case LEX_TYPEEQUAL      : return "===";
		case LEX_NEQUAL         : return "!=";
		case LEX_NTYPEEQUAL     : return "!==";
		case LEX_LEQUAL         : return "<=";
		case LEX_LSHIFT         : return "<<";
		case LEX_LSHIFTEQUAL    : return "<<=";
		case LEX_GEQUAL         : return ">=";
		case LEX_RSHIFT         : return ">>";
		case LEX_RSHIFTUNSIGNED : return ">>";
		case LEX_RSHIFTEQUAL    : return ">>=";
		case LEX_PLUSEQUAL      : return "+=";
		case LEX_MINUSEQUAL     : return "-=";
		case LEX_MULTIEQUAL     : return "*=";
		case LEX_DIVEQUAL 	    : return "/=";
		case LEX_MODEQUAL   	  : return "%=";
		case LEX_PLUSPLUS       : return "++";
		case LEX_MINUSMINUS     : return "--";
		case LEX_ANDEQUAL       : return "&=";
		case LEX_ANDAND         : return "&&";
		case LEX_OREQUAL        : return "|=";
		case LEX_OROR           : return "||";
		case LEX_XOREQUAL       : return "^=";
															// reserved words
		case LEX_R_IF           : return "if";
		case LEX_R_ELSE         : return "else";
		case LEX_R_DO           : return "do";
		case LEX_R_WHILE        : return "while";
		case LEX_R_FOR          : return "for";
		case LEX_R_BREAK        : return "break";
		case LEX_R_CONTINUE     : return "continue";
		case LEX_R_FUNCTION     : return "function";
		case LEX_R_CLASS     		: return "class";
		case LEX_R_RETURN       : return "return";
		case LEX_R_CONST        : return "CONST";
		case LEX_R_VAR          : return "var";
		case LEX_R_TRUE         : return "true";
		case LEX_R_FALSE        : return "false";
		case LEX_R_NULL         : return "null";
		case LEX_R_UNDEFINED    : return "undefined";
		case LEX_R_NEW          : return "new";
		case LEX_R_INCLUDE      : return "include";
	}
	return "?[UNKNOW]";
}

void CScriptLex::getNextCh() {
	currCh = nextCh;
	if (dataPos < dataEnd){
		nextCh = data[dataPos];
	}else{
		nextCh = 0;
	}
	dataPos++;
}

void CScriptLex::getNextToken() {
	tk = LEX_EOF;
	tkStr.clear();

	while (currCh && isWhitespace(currCh)){
		getNextCh();
	}
	// newline comments
	if (currCh=='/' && nextCh=='/') {
		while (currCh && currCh!='\n'){
			getNextCh();
		}
		getNextCh();
		getNextToken();
		return;
	}
	// block comments
	if (currCh=='/' && nextCh=='*') {
		while (currCh && (currCh!='*' || nextCh!='/')) {
			getNextCh();
		}
		getNextCh();
		getNextCh();
		getNextToken();
		return;
	}
	// record beginning of this token(pre-read 2 chars );
	tokenStart = dataPos-2;
	// tokens
	if (isAlpha(currCh)) { //  IDs
		while (isAlpha(currCh) || isNumeric(currCh)) {
			tkStr += currCh;
			getNextCh();
		}
		tk = LEX_ID;
		if (tkStr=="if")        tk = LEX_R_IF;
		else if (tkStr=="else")      tk = LEX_R_ELSE;
		else if (tkStr=="do")        tk = LEX_R_DO;
		else if (tkStr=="while")     tk = LEX_R_WHILE;
		else if (tkStr=="include")   tk = LEX_R_INCLUDE;
		else if (tkStr=="for")       tk = LEX_R_FOR;
		else if (tkStr=="break")     tk = LEX_R_BREAK;
		else if (tkStr=="continue")  tk = LEX_R_CONTINUE;
		else if (tkStr=="function")  tk = LEX_R_FUNCTION;
		else if (tkStr=="class") 		 tk = LEX_R_CLASS;
		else if (tkStr=="return")    tk = LEX_R_RETURN;
		else if (tkStr=="var")       tk = LEX_R_VAR;
		else if (tkStr=="const")     tk = LEX_R_CONST;
		else if (tkStr=="true")      tk = LEX_R_TRUE;
		else if (tkStr=="false")     tk = LEX_R_FALSE;
		else if (tkStr=="null")      tk = LEX_R_NULL;
		else if (tkStr=="undefined") tk = LEX_R_UNDEFINED;
		else if (tkStr=="new")       tk = LEX_R_NEW;
		else if (tkStr=="throw")     tk = LEX_R_THROW;
		else if (tkStr=="try")    	 tk = LEX_R_TRY;
		else if (tkStr=="catch")     tk = LEX_R_CATCH;
	} else if (isNumeric(currCh)) { // Numbers
		bool isHex = false;
		if (currCh=='0') { tkStr += currCh; getNextCh(); }
		if (currCh=='x') {
			isHex = true;
			tkStr += currCh; getNextCh();
		}
		tk = LEX_INT;
		while (isNumeric(currCh) || (isHex && isHexadecimal(currCh))) {
			tkStr += currCh;
			getNextCh();
		}
		if (!isHex && currCh=='.') {
			tk = LEX_FLOAT;
			tkStr += '.';
			getNextCh();
			while (isNumeric(currCh)) {
				tkStr += currCh;
				getNextCh();
			}
		}
		// do fancy e-style floating point
		if (!isHex && (currCh=='e'||currCh=='E')) {
			tk = LEX_FLOAT;
			tkStr += currCh;
			getNextCh();
			if (currCh=='-') {
				tkStr += currCh;
				getNextCh();
			}
			while (isNumeric(currCh)) {
				tkStr += currCh;
				getNextCh();
			}
		}
	} else if (currCh=='"') {
		// strings...
		getNextCh();
		while (currCh && currCh!='"') {
			if (currCh == '\\') {
				getNextCh();
				switch (currCh) {
					case 'n' : tkStr += '\n'; break;
					case 'r' : tkStr += '\r'; break;
					case 't' : tkStr += '\t'; break;
					case '"' : tkStr += '"'; break;
					case '\\' : tkStr += '\\'; break;
					default: tkStr += currCh;
				}
			} else {
				tkStr += currCh;
			}
			getNextCh();
		}
		getNextCh();
		tk = LEX_STR;
	} else if (currCh=='\'') {
		// strings again...
		getNextCh();
		while (currCh && currCh!='\'') {
			if (currCh == '\\') {
				getNextCh();
				switch (currCh) {
					case 'n' : tkStr += '\n'; break;
					case 'a' : tkStr += '\a'; break;
					case 'r' : tkStr += '\r'; break;
					case 't' : tkStr += '\t'; break;
					case '\'' : tkStr += '\''; break;
					case '\\' : tkStr += '\\'; break;
					case 'x' : { // hex digits
											 char buf[3] = "??";
											 getNextCh(); buf[0] = currCh;
											 getNextCh(); buf[1] = currCh;
											 tkStr += (char)strtol(buf,0,16);
										 } break;
					default: if (currCh>='0' && currCh<='7') {
										 // octal digits
										 char buf[4] = "???";
										 buf[0] = currCh;
										 getNextCh(); buf[1] = currCh;
										 getNextCh(); buf[2] = currCh;
										 tkStr += (char)strtol(buf,0,8);
									 } else
										 tkStr += currCh;
				}
			} else {
				tkStr += currCh;
			}
			getNextCh();
		}
		getNextCh();
		tk = LEX_STR;
	} else {
		// single chars
		tk = (LEX_TYPES)currCh;
		if (currCh) getNextCh();
		if (tk=='=' && currCh=='=') { // ==
			tk = LEX_EQUAL;
			getNextCh();
			if (currCh=='=') { // ===
				tk = LEX_TYPEEQUAL;
				getNextCh();
			}
		} else if (tk=='!' && currCh=='=') { // !=
			tk = LEX_NEQUAL;
			getNextCh();
			if (currCh=='=') { // !==
				tk = LEX_NTYPEEQUAL;
				getNextCh();
			}
		} else if (tk=='<' && currCh=='=') {
			tk = LEX_LEQUAL;
			getNextCh();
		} else if (tk=='<' && currCh=='<') {
			tk = LEX_LSHIFT;
			getNextCh();
			if (currCh=='=') { // <<=
				tk = LEX_LSHIFTEQUAL;
				getNextCh();
			}
		} else if (tk=='>' && currCh=='=') {
			tk = LEX_GEQUAL;
			getNextCh();
		} else if (tk=='>' && currCh=='>') {
			tk = LEX_RSHIFT;
			getNextCh();
			if (currCh=='=') { // >>=
				tk = LEX_RSHIFTEQUAL;
				getNextCh();
			} else if (currCh=='>') { // >>>
				tk = LEX_RSHIFTUNSIGNED;
				getNextCh();
			}
		}  else if (tk=='+' && currCh=='=') {
			tk = LEX_PLUSEQUAL;
			getNextCh();
		}  else if (tk=='-' && currCh=='=') {
			tk = LEX_MINUSEQUAL;
			getNextCh();
		}  else if (tk=='*' && currCh=='=') {
			tk = LEX_MULTIEQUAL;
			getNextCh();
		}  else if (tk=='/' && currCh=='=') {
			tk = LEX_DIVEQUAL;
			getNextCh();
		}  else if (tk=='%' && currCh=='=') {
			tk = LEX_MODEQUAL;
			getNextCh();
		}  else if (tk=='+' && currCh=='+') {
			tk = LEX_PLUSPLUS;
			getNextCh();
		}  else if (tk=='-' && currCh=='-') {
			tk = LEX_MINUSMINUS;
			getNextCh();
		} else if (tk=='&' && currCh=='=') {
			tk = LEX_ANDEQUAL;
			getNextCh();
		} else if (tk=='&' && currCh=='&') {
			tk = LEX_ANDAND;
			getNextCh();
		} else if (tk=='|' && currCh=='=') {
			tk = LEX_OREQUAL;
			getNextCh();
		} else if (tk=='|' && currCh=='|') {
			tk = LEX_OROR;
			getNextCh();
		} else if (tk=='^' && currCh=='=') {
			tk = LEX_XOREQUAL;
			getNextCh();
		}
	}
	/* This isn't quite right yet */
	tokenLastEnd = tokenEnd;
	tokenEnd = dataPos-3;
}

std::string CScriptLex::getSubString(int lastPosition) {
	int lastCharIdx = tokenLastEnd+1;
	if (lastCharIdx < dataEnd) {
		/* save a memory alloc by using our data array to create the
			 subistring */
		char old = data[lastCharIdx];
		data[lastCharIdx] = 0;
		std::string value = &data[lastPosition];
		data[lastCharIdx] = old;
		return value;
	} else {
		return std::string(&data[lastPosition]);
	}
}

CScriptLex *CScriptLex::getSubLex(int lastPosition) {
	int lastCharIdx = tokenLastEnd+1;
	if (lastCharIdx < dataEnd)
		return new CScriptLex(this, lastPosition, lastCharIdx);
	else
		return new CScriptLex(this, lastPosition, dataEnd );
}

void CScriptLex::getPosition(int* line, int *col, int pos) {
	if (pos<0) pos=tokenLastEnd;
	int l = 1;
	int c  = 1;
	for (int i=0;i<pos;i++) {
		char ch;
		if (i < dataEnd){
			ch = data[i];
		}else{
			ch = 0;
		}
		c++;
		if (ch=='\n') {
			l++;
			c = 1;
		}
	}
	*line = l;
	*col = c;
}

std::string CScriptLex::getPosition(int pos) {
	int line = 1;
	int col;

	getPosition(&line, &col, pos);
	std::stringstream ss;
	ss << "(line: " << line << ", col: " << col << ")";
	return ss.str();
}

