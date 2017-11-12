#include "Var.h"
#include <stdio.h>
#include <string.h>

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
	data = NULL;
	regular = true;
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
								 /*int nCh = ((int)nStr[i]) &0xFF;
								 if (nCh<32 || nCh>127) {
									 char bytes[5];
									 snprintf(bytes, 5, "\\x%02X", nCh);
									 replaceWith = bytes;
								 } else {
								 }
								 */
								 replace=false;
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
	if (isFunction()) {
		std::string funcStr;
		funcStr += "function (";
		// get list of parameters
		int sz = 0;
		if(func != NULL)
			sz = (int)func->args.size();
		for(int i=0; i<sz; ++i) {
			string n = func->args[i];
			funcStr += n;
			if ((i+1) < sz) {
				funcStr += ",";
			}
		}
		// add function body
		funcStr += ") {}";
		funcStr += getString();
		return funcStr;
	}
	// if it is a std::string then we quote it
	if (isString()){
		return getJSString(getString());
	}
	return getString();
}

string BCVar::getJSON(const string& prefix, int level) {
	string destination;
	string linePrefix = prefix;
	for(int i=0; i<level; ++i) {
		linePrefix += prefix;
	}

	if (isObject()) {
		// children - handle with bracketed list
		int sz = (int)children.size();
		if(sz > 0)
			destination += "{\n";
		else
			destination += "{";
		for(int i=0; i<sz; ++i) {
			BCNode* n = children[i];
			string name = n->name;
			size_t pos = name.find("$");
			if(pos != string::npos)
				name = name.substr(0, pos);
			destination += linePrefix;
			destination += getJSString(name);
			destination += " : ";
			destination += n->var->getJSON(prefix, level+1);
			if ((i+1) < sz) {
				destination  += ",\n";
			}
		}
		if(sz > 0) {
			destination += "\n";
			destination += linePrefix;
		}
		destination += "}";
	} 
	else if (isArray()) {
//		destination += "[\n";
		destination += "[";
		int len = getChildrenNum();
		if (len>10000) len=10000; // we don't want to get stuck here!
		for (int i=0;i<len;i++) {
			destination += getArrayIndex(i)->getJSON(prefix, level);
			if (i<len-1) destination  += ", ";
		}
//		destination += "\n";
		destination += "]";
	} else {
		// no children or a function... just write value directly
		destination += getParsableString();
	}

	return destination;
}

