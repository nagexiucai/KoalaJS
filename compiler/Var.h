#ifndef BC_VAR
#define BC_VAR

#include <string>
#include <vector>

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
	string name;
	BCValueT value;
	

	inline BCVar() {
		refs = 0;
		value.pointV = NULL;
		value.destroyFunc = NULL;
		clean();
	}

	inline BCVar(const string& name) {
		refs = 0;
		value.pointV = NULL;
		value.destroyFunc = NULL;
		clean();

		this->name = name;
	}

	inline ~BCVar() {
		clean();
	}
	
	inline void clean() {
		name = "";

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
	
	inline void unref() {
		refs--;
		if(refs == 0) {
			delete this;
		}
	}

	inline BCVar* get(int index) {
		if(index < 0 || index >= children.size())
			return NULL;
		return children[index];
	}

	inline BCVar* get(const string& name) {
		for(int i=0; i<children.size(); ++i) {
			if(children[i] != NULL && children[i]->name == name)
				return children[i];
		}
		return NULL;
	}
	
	inline BCVar* add(const string& name) {
		BCVar* ret = get(name);
		if(ret != NULL)
			return ret;

		ret = new BCVar(name);
		if(ret == NULL)
			return ret;

		children.push_back(ret->ref());
		return ret;
	}
};

#endif
