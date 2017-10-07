#ifndef BC_BYTECODE_HH
#define BC_BYTECODE_HH

#include <string>
#include <vector>
#include "Instr.h"

using namespace std;

class Bytecode {
	PC cindex;
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
	void add(PC ins);

	void strs();

	PC gen(Instr instr, const string& str = "");

	PC bytecode(Instr instr, const string& str = "");
	
	string getStr(int i);

	unsigned short getStrIndex(const string& n);

	inline PC  getPC() {
		return cindex;
	}

	inline PC  getInstr(PC index) {
		if(index >= cindex)
			return 0;

		return codeBuf[index];
	}
	
	/** set jump instructioin , jump to current pc
	 @param index, index of instruction.
	 @param jmp, jump if true, or njump if false.
	 @param back, offset back if true, or forward if false.
	*/
	void jump(PC anchor, bool jmp = true, bool back = false);

	inline PC reserve() {
		add(INS(INSTR_NIL, 0));
		return cindex-1;
	}
	
	void dump();
};

#endif
