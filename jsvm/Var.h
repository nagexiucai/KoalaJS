#ifndef BC_VAR
#define BC_VAR

#include <string>
#include <vector>
#include <stdint.h>
#include <sstream>  
#include <iostream>  
#include "Instr.h"

#define CLS_ARR "Array"
#define CLS_OBJECT "Object"
#define THIS "this"
#define PROTOTYPE "prototype"
#define RETURN "return"

using namespace std;

class BCVar;

typedef void (*JSDestroy)(void *p);
typedef void (*JSCallback)(BCVar *var, void *userdata);

class StackItem {
public:
	bool isNode;

	inline StackItem() {
		isNode = false;
	}
};


class BCNode : public StackItem {
public:
	BCVar *var;
	bool beConst;
	string name;

	BCNode(const string& n, BCVar* v);
	
	~BCNode();
	
	void replace(BCVar* v);
};

/** struct for function type
argNum : arguments number
pc : function start PC
 */
typedef struct STFunc {
	int argNum;
	JSCallback native;
	PC pc;
	BCVar* args;
	BCNode* thisNode;
	BCNode* returnNode;

	STFunc();
	~STFunc();
	void resetArgs();
} FuncT;

const int NO_BYTES = -1;

class BCVar : public StackItem {
	int refs;
	vector<BCNode*> children;	

	int intV;
	double floatV;
	string stringV;
	void* pointV;
	JSDestroy destroyFunc;
	JSCallback nativeConstructor;
	bool needDestroy;

	FuncT *func;

	inline void init() {
		intV = 0;
		floatV = 0.0;
		pointV = NULL;
		destroyFunc = NULL;
		needDestroy = false;
		type =UNDEF;
		func = NULL;
		nativeConstructor = NULL;
	}

public:
	const static uint8_t UNDEF = 0;
	const static uint8_t INT = 1;
	const static uint8_t FLOAT = 2;
	const static uint8_t STRING = 3;
	const static uint8_t POINT = 4;
	const static uint8_t FUNC = 5;
	const static uint8_t NFUNC = 6;
	const static uint8_t OBJECT = 7;
	const static uint8_t CLASS = 8;
	const static uint8_t BYTES = 9;
	const static uint8_t ARRAY = 10;

	uint8_t type;

	inline BCVar() {
		refs = 0;
		init();
		clean();
	}

	inline BCVar(int v) {
		refs = 0;
		init();
		setInt(v);
	}

	inline BCVar(float v) {
		refs = 0;
		init();
		setFloat(v);
	}

	inline BCVar(const string& v) {
		refs = 0;
		init();
		setString(v);
	}

	inline void setInt(int v) {
		if(type == INT) {
			intV = v;
		}
		else {
			clean();
			intV = v;
			type = INT;
		}
	}

	inline void setDouble(double v) {
		setFloat(v);
	}

	inline void setFloat(float v) {
		if(type == FLOAT) {
			floatV = v;
		}
		else {
			clean();
			floatV = v;
			type = FLOAT;
		}
	}

	inline void setString(const string& v) {
		if(type == STRING) {
			stringV = v;
		}
		else {
			clean();
			stringV = v;
			type = STRING;
		}
	}

	inline void setArray() {
		clean();
		type = ARRAY;
	}
	
	inline void setArrayIndex(int idx, BCVar *value) {
		BCNode* n = getChildOrCreate(idx);
		n->replace(value);
	}
		
	inline int getArrayLength() {
		return getChildrenNum();
	}

	inline BCVar* getArrayIndex(int idx) {
		BCNode* n = getChildOrCreate(idx);
		return n->var;
	}

	//if size >= 0, means val is a byte bytes, pData is a byte bytes point, and intData is the size of it.
	inline void setPoint(void* p, int size, JSDestroy destroy, bool needDestroy) {
		if(type != OBJECT)
			clean();

		if(size == NO_BYTES) {
			intV = 0;
		}
		else {
			type = BYTES;
			intV = size;
		}
		
		pointV = p;
		this->destroyFunc = destroy;
		this->needDestroy = needDestroy;
	}


	inline void setFunction(int argNum, PC pc, JSCallback native = NULL) {
		clean();
		func = new FuncT();
		func->argNum = argNum;

		if(native != NULL) {
			func->native = native;
			type = BCVar::NFUNC;
		}
		else {
			func->pc = pc;
			type = BCVar::FUNC;
		}
	}

	inline bool isNumber() { return (type == INT || type == FLOAT); }
	inline bool isInt() { return type == INT; }
	inline bool isUndefined() { return type == UNDEF; }
	inline bool isFloat() { return type == FLOAT; }
	inline bool isDouble() { return type == FLOAT; }
	inline bool isString() { return type == STRING; }
	inline bool isBytes() { return type == BYTES; }
	inline bool isObject() { return (type == OBJECT || type == CLASS); }
	inline bool isArray() { return type == ARRAY; }
	inline bool isFunction() { return (type == FUNC || type == NFUNC); }

	inline void* getPoint()  { return pointV; }
	inline FuncT* getFunc() { return func; }
	inline int getInt()  { return type == FLOAT ? (int)floatV : intV; }
	inline float getFloat()  { return type == INT ? (float)intV : floatV; }
	inline double getDouble()  { return type == INT ? (double)intV : (double)floatV; }

	inline string getString()  {
		std::stringstream ss;
		if(type == INT) 
			ss << intV;
		else if(type == FLOAT) 
			ss << floatV;
		else if(type == STRING) 
			ss << stringV;
		else if(isObject() || type == ARRAY) 
			ss << getJSON();
			
		return ss.str(); 
	}


	inline ~BCVar() {
		clean();
		removeAllChildren();
	}

	inline void removeAllChildren() {
		for(int i=0; i<children.size(); ++i) {
			delete children[i];
		}
		children.clear();
	}
	
	inline void clean() {
		if(pointV != NULL) {
			if(destroyFunc != NULL)
				destroyFunc(pointV);
			else
				delete (unsigned char*)pointV;
			pointV = NULL;
			destroyFunc = NULL;
		}	
	
		if(func != NULL) {
			delete func;
			func = NULL;
		}

		init();
	}

	inline int getRefs() { return refs; }

	inline BCVar* ref() {
		refs++;
		return this;
	}
	
	inline BCVar* unref() {
		refs--;
		if(refs == 0) {
			delete this;
			return NULL;
		}
		return this;
	}

	//get function parameter var 
	inline BCVar* getParameter(const string& name) {
		BCNode* n = getChild(name);
		return (n == NULL ? NULL : n->var);
	}

	//get function return var 
	inline BCVar* getReturnVar() {
		if(func == NULL) 
			return NULL;

		return func->returnNode->var;
	}

	//set function return var 
	inline BCVar* setReturnVar(BCVar* v) {
		if(func == NULL) 
			return NULL;

		func->returnNode->replace(v);
		return v;
	}

	//get function this var 
	inline BCVar* getThisVar() {
		if(func == NULL) 
			return NULL;

		return func->thisNode->var;
	}

	//set function this var 
	inline BCVar* setThisVar(BCVar* v) {
		if(func == NULL) 
			return NULL;

		func->thisNode->replace(v);
		return v;
	}

	inline int getChildrenNum() {
		return children.size();
	}

	//get child var by index
	inline BCNode* getChild(int index) {
		if(index < 0 || index >= children.size())
			return NULL;
		return children[index];
	}

	//get child var by index or create, will fill all missed children
	inline BCNode* getChildOrCreate(int index) {
		if(index < 0)
			return NULL;

 		int sz = children.size();
		if(index >= sz) {
			for(int i=sz; i<=index; ++i) {
				BCNode* n = new BCNode("", new BCVar());
				children.push_back(n);
			}
		}
		return children[index];
	}

	/*get child var by name 
	@name , child name;
	*/
	inline BCNode* getChild(const string& name) {
		if(func != NULL) {
			if(name == THIS) {
				return func->thisNode;
			}
			else if(name == RETURN) {
				return func->returnNode;
			}
			else {
				BCNode* r = func->args->getChild(name);
				if(r != NULL)
					return r;
			}
		}

		int sz = children.size();
		for(int i=0; i<sz; ++i) {
			BCNode* r = children[i];
			if(r != NULL && r->name == name)
				return r;
		}
		return NULL;
	}

	/*get child var by name , if not found, create one
	@name , child name;
	*/
	inline BCNode* getChildOrCreate(const string& name) {
		if(func != NULL) {
			BCNode* r = func->args->getChild(name);
			if(r != NULL)
				return r;
		}

		for(int i=0; i<children.size(); ++i) {
			if(children[i] != NULL && children[i]->name == name)
				return children[i];
		}

		BCVar* v = new BCVar();
		BCNode* ret = new BCNode(name, v);
		children.push_back(ret);
		return ret;
	}

	//add child, this function doesn't check existed or not!!
	inline BCNode* addChild(const string& name, BCVar* v = NULL) {
		if(v == NULL)
			v = new BCVar();

		BCNode* ret = new BCNode(name, v);
		children.push_back(ret);
		return ret;
	}
	
	inline JSCallback getNativeConstructor() { return nativeConstructor; }

	inline void setNativeConstructor(JSCallback nc) {
		nativeConstructor = nc;
	}

	string getParsableString();
	string getJSString(const string &str);
	string getJSON(const string& linePrefix = "");
};

//define BCVar as CScriptVar for native functions compatible with Interpretor
typedef BCVar CScriptVar;

#endif
