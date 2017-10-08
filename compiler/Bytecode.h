#ifndef BC_BYTECODE_HH
#define BC_BYTECODE_HH

#include <string>
#include <vector>
#include "Instr.h"

using namespace std;

class CScriptException {
	public:
		std::string text;
		CScriptException(const std::string &exceptionText);
};

class Bytecode {
	PC cindex;
	vector<string> strTable;
	PC *codeBuf;
	uint32_t bufSize;

public:
	const static uint32_t BUF_SIZE = 1024;
	Bytecode() {
		cindex = 0;
		codeBuf = NULL;
		bufSize = 0;
	}

	inline void reset() {
		strTable.clear();

		if(codeBuf != NULL) {
			delete []codeBuf;
			codeBuf = NULL;
		}
		bufSize = 0;
		cindex = 0;
	}

	inline PC* getCode(uint32_t &sz) {
		sz = cindex;
		return codeBuf;
	}

	~Bytecode() {
		reset();
	}
	void add(PC ins);

	void strs();

	PC gen(OpCode instr, const string& str = "");

	PC bytecode(OpCode instr, const string& str = "");

	string getStr(int i);

	uint16_t getStrIndex(const string& n);

	inline PC  getPC() {
		return cindex;
	}

	inline PC  getOpCode(PC index) {
		if(index >= cindex)
			return 0;

		return codeBuf[index];
	}
	
	/** set reserved instructioin , offset to current pc
	 @param anchar, anchor of reserved instruction.
	 @param opCode, opCode.
	 @param back, offset back if true, or forward if false.
	*/
	void set(PC anchor, OpCode op, bool back = false);

	/** reserved current instruction for unknow offset*/
	inline PC reserve() {
		add(INS(INSTR_NIL, 0xFFFF));
		return cindex-1;
	}
	
	void dump();
	
	bool toFile(const std::string& fname);

	bool fromFile(const std::string& fname);
};

#endif
