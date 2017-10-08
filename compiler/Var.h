#ifndef BC_VAR
#define BC_VAR

#include <string>
#include <vector>
#include <stdint.h>
#include "Instr.h"

using namespace std;

class BCVar;

typedef void (*VarDestroy)(void *p);
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
typedef struct {
	int argNum;
	JSCallback native;
	PC pc;
} FuncT;

class BCVar : public StackItem {
	int refs;
	vector<BCNode*> children;	

	int intV;
	double floatV;
	string stringV;
	void* pointV;
	VarDestroy destroyFunc;

	FuncT func;

public:
	const static uint8_t INT = 0;
	const static uint8_t FLOAT = 1;
	const static uint8_t STRING = 2;
	const static uint8_t POINT = 3;
	const static uint8_t FUNC = 4;
	const static uint8_t NFUNC = 5;
	const static uint8_t UNDEF = 6;

	uint8_t type;

	inline BCVar() {
		refs = 0;
		pointV = NULL;
		destroyFunc = NULL;
		type =UNDEF;
		clean();
	}

	inline BCVar(int v) {
		refs = 0;
		pointV = NULL;
		destroyFunc = NULL;
		type =UNDEF;
		setInt(v);
	}

	inline BCVar(float v) {
		refs = 0;
		pointV = NULL;
		destroyFunc = NULL;
		type =UNDEF;
		setFloat(v);
	}

	inline BCVar(const string& v) {
		refs = 0;
		pointV = NULL;
		destroyFunc = NULL;
		type =UNDEF;
		setString(v);
	}

	inline void setInt(int v) {
		clean();
		intV = v;
		type = INT;
	}

	inline void setFloat(float v) {
		clean();
		floatV = v;
		type = FLOAT;
	}

	inline void setString(const string& v) {
		clean();
		stringV = v;
		type = STRING;
	}

	inline void setFunction(int argNum, PC pc, JSCallback native = NULL) {
		clean();
		func.argNum = argNum;

		if(native != NULL) {
			func.native = native;
			type = BCVar::NFUNC;
		}
		else {
			func.pc = pc;
			type = BCVar::FUNC;
		}
	}
	inline FuncT* getFunc() { return &func; }
	inline int getInt()  { return intV; }
	inline float getFloat()  { return floatV; }
	inline string getString()  { return stringV; }


	inline ~BCVar() {
		clean();
	}
	
	inline void clean() {
		type = INT;

		intV = 0;
		floatV = 0.0;
		stringV = "";

		if(pointV != NULL) {
			if(destroyFunc != NULL)
				destroyFunc(pointV);
			else
				delete (unsigned char*)pointV;
			pointV = NULL;
			destroyFunc = NULL;
		}	

		for(int i=0; i<children.size(); ++i) {
			delete children[i];
		}
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

	//get child var by index
	inline BCNode* getChild(int index) {
		if(index < 0 || index >= children.size())
			return NULL;
		return children[index];
	}

	/*get child var by name 
	@name , child name;
	*/
	inline BCNode* getChild(const string& name) {
		for(int i=0; i<children.size(); ++i) {
			if(children[i] != NULL && children[i]->name == name)
				return children[i];
		}
		return NULL;
	}

	/*get child var by name , if not found, create one
	@name , child name;
	*/
	inline BCNode* getChildOrCreate(const string& name) {
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
};

#endif
