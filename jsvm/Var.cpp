#include "Var.h"
	#include <stdio.h>

	BCNode::BCNode(const string& n, BCVar* v) {
		var = v->ref();
		name = n;
		beConst = false;
		isNode = true;
	}

	BCNode::~BCNode() {
		if(var != NULL)
			var->unref();
	}

	void BCNode::replace(BCVar* v) {
		BCVar* old = var;
		var = v->ref();
		old->unref();
	}

	STFunc::STFunc() {
		args = new BCVar();
		thisVar = new BCNode(THIS, new BCVar());
		returnVar = new BCNode(RETURN, new BCVar());
	}

	STFunc::~STFunc() {
		delete args;
		delete thisVar;
		delete returnVar;
	}

	void STFunc::resetArgs() {
		int sz = argNum;
		BCVar* v = new BCVar();
		for(int i=0; i<sz; ++i) {
			BCNode* n = args->getChild(i);
			if(n)
				n->replace(v);
		}

		thisVar->replace(v);
		returnVar->replace(v);
	}

////////////////////////////////////////////////////////////////////////////////
/// convert the given std::string into a quoted std::string suitable for javascript
string BCVar::getJSString(const string &str) {
	string nStr = str;
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

string BCVar::getParsableString() {
	// Numbers can just be put in directly
	if (isInt() || isFloat()) {
		return getString();
	}
	if (isFunction()) {
		std::string funcStr;
		funcStr += "function (";
		// get list of parameters
		int sz = func->argNum;

		for(int i=0; i<sz; ++i) {
			BCNode* n = func->args->getChild(i);
			funcStr += n->name;
			if ((i+1) < sz) {
				funcStr += ",";
			}
		}
		// add function body
		funcStr += ") ";
		funcStr += getString();
		return funcStr;
	}
	// if it is a std::string then we quote it
	if (isString()){
		return getJSString(getString());
	}
	return "undefined";
}

string BCVar::getJSON(const string& linePrefix) {
	string destination;

	if (isObject()) {
		std::string indentedLinePrefix = linePrefix+"  ";
		// children - handle with bracketed list
		destination += "{ \n";
		int sz = children.size();
		for(int i=0; i<sz; ++i) {
			BCNode* n = children[i];
			destination += indentedLinePrefix;
			destination += getJSString(n->name);
			destination += " : ";
			destination += n->var->getJSON(indentedLinePrefix);
			if ((i+1) < sz) {
				destination  += ",\n";
			}
		}
		destination += "\n";
		destination += linePrefix;
		destination +=  "}";
	} 
	else if (isArray()) {
		std::string indentedLinePrefix = linePrefix+"  ";
		destination += "[\n";
		/*int len = getArrayLength();
			if (len>10000) len=10000; // we don't want to get stuck here!

			for (int i=0;i<len;i++) {
			getArrayIndex(i)->getJSON(destination, indentedLinePrefix);
			if (i<len-1) destination  += ",\n";
			}
		 */

		destination += "\n";
		destination += linePrefix;
		destination += "]";
	} else {
		// no children or a function... just write value directly
		destination += getParsableString();
	}
	
	return destination;
}

