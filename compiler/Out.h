#ifndef BC_OUT_HH
#define BC_OUT_HH

#include <string>
#include <vector>
#include "Instr.h"

using namespace std;

class BCOut {
	int cindex;
	vector<string> strTable;

public:
	BCOut() {
		cindex = 0;
	}

	void strs();

	void out(Instr instr, const string& str = "");

	unsigned int bytecode(Instr instr, const string& str = "");
	
	inline string str(int i) {
		if(i<0 || i>=strTable.size())
			return "";
		return strTable[i];
	}	

	inline unsigned short str(const string& n) {
		unsigned short sz = strTable.size();
		for(unsigned short i=0; i<sz; ++i) {
			if(strTable[i] == n)
				return i;
		}
		strTable.push_back(n);
		return sz;
	}	
};

#endif
