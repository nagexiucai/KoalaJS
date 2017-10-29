#ifndef BC_BYTECODE_HH
#define BC_BYTECODE_HH

#include <string>
#include <vector>
#include <string.h>
#include "Instr.h"

#ifndef TRACE
#define TRACE(...) fprintf (stdout, __VA_ARGS__)
#endif // TRACE

#ifndef ERR
#define ERR(...) fprintf (stderr, __VA_ARGS__)
#endif // TRACE

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

	string strs();

	PC gen(OpCode instr, const string& str = "");

	PC gen(OpCode instr, int intV);

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
	 @param target, target pc.
	*/
	void setJmp(PC anchor, OpCode op, PC target = 0xFFFFFFFF);

	/** add instructioin , offset to current pc
	 @param anchar, anchor of reserved instruction.
	 @param opCode, opCode.
	 @param target, target pc.
	*/
	void addJmp(PC anchor, OpCode op, PC target = 0xFFFFFFFF);



	/** reserved current instruction for unknow offset*/
	inline PC reserve() {
		add(INS(INSTR_NIL, 0xFFFF));
		return cindex-1;
	}

	inline void back() {
		cindex--;
	}
	
	string dump();
	
	bool toFile(const std::string& fname);

	bool fromFile(const std::string& fname);

	inline void clone(Bytecode* bc) {
		bc->reset();
		if(bc == NULL || cindex == 0)
			return;
		bc->cindex = cindex;
		bc->strTable = strTable;
		bc->codeBuf = new PC[cindex];
		memcpy(bc->codeBuf, codeBuf, cindex*sizeof(PC));
	}
};

#endif
