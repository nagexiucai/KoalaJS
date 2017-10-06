#ifndef BC_BYTECODE_HH
#define BC_BYTECODE_HH

#include <string>
#include <vector>
#include "Instr.h"

using namespace std;

class Bytecode {
	unsigned int cindex;
	vector<string> strTable;
	unsigned int *codeBuf;
	unsigned int bufSize;

public:
	const static unsigned int BUF_SIZE = 1024;
	Bytecode() {
		cindex = 0;
		codeBuf = NULL;
		bufSize = 0;
	}

	~Bytecode() {
		if(codeBuf != NULL) {
			delete []codeBuf;
		}
	}
	void add(unsigned int ins);

	void strs();

	void gen(Instr instr, const string& str = "");

	unsigned int bytecode(Instr instr, const string& str = "");
	
	inline string getStr(int i) {
		if(i<0 || i>=strTable.size())
			return "";
		return strTable[i];
	}	

	inline unsigned short getStrIndex(const string& n) {
		unsigned short sz = strTable.size();
		for(unsigned short i=0; i<sz; ++i) {
			if(strTable[i] == n)
				return i;
		}
		strTable.push_back(n);
		return sz;
	}	
	
	void dump();
};

#endif
