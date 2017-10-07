#ifndef BC_VAR
#define BC_VAR

#include <string>
#include <vector>
#include <stdint.h>

using namespace std;

typedef void (*VarDestroy)(void *p);

typedef struct BCValue {
	int intV;
	double floatV;
	string stringV;
	void* pointV;
	VarDestroy destroyFunc;
} BCValueT;

class BCVar {
	int refs;
	vector<BCVar*> children;	

public:
	const static uint8_t INT = 0;
	const static uint8_t FLOAT = 1;
	const static uint8_t STRING = 2;
	const static uint8_t POINT = 3;
	const static uint8_t FUNC = 4;

	string name;
	uint8_t type;
	BCValueT value;
	bool beConst;

	inline BCVar() {
		refs = 0;
		value.pointV = NULL;
		value.destroyFunc = NULL;
		clean();
	}

	inline BCVar(const string& name, uint8_t type = INT) {
		refs = 0;
		value.pointV = NULL;
		value.destroyFunc = NULL;
		clean();

		this->type = type;
		this->name = name;
	}

	inline ~BCVar() {
		clean();
	}
	
	inline void clean() {
		name = "";
		beConst = false;
		type = INT;

		value.intV = 0;
		value.floatV = 0.0;
		value.stringV = "";

		if(value.pointV != NULL) {
			if(value.destroyFunc != NULL)
				value.destroyFunc(value.pointV);
			else
				delete (unsigned char*)value.pointV;
			value.pointV = NULL;
			value.destroyFunc = NULL;
		}	

		for(int i=0; i<children.size(); ++i) {
			children[i]->unref();
		}
	}

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
	inline BCVar* getChild(int index) {
		if(index < 0 || index >= children.size())
			return NULL;
		return children[index];
	}

	//get child var by name , if not found, create one
	inline BCVar* getChild(const string& name, bool create = false) {
		for(int i=0; i<children.size(); ++i) {
			if(children[i] != NULL && children[i]->name == name)
				return children[i];
		}

		if(create) {
			BCVar* ret = new BCVar(name);
			children.push_back(ret->ref());
			return ret;
		}
		return NULL;
	}

	//add child, this function doesn't check existed or not!!
	inline BCVar* addChild(const string& name) {
		BCVar* ret = new BCVar(name);
		children.push_back(ret->ref());
		return ret;
	}

	inline void copyValue(BCVar* src) {
	}
};

#endif
