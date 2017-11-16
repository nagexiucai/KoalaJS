#ifndef BC_BYTECODE_HH
#define BC_BYTECODE_HH

#include <string>
#include <vector>
#include <string.h>
#include <stdio.h>
#include "Instr.h"

#ifndef TRACE
#define TRACE(...) fprintf (stdout, __VA_ARGS__)
#endif // TRACE

#ifndef ERR
#define ERR(...) fprintf (stderr, __VA_ARGS__)
#endif // TRACE

using namespace std;

class Compiler;

typedef uint32_t POS;
typedef struct STDBInfo {
	vector<string> fileTable;
	vector<vector<string> > fileLines;
	uint32_t bufSize;
	uint32_t *posBuf;
	uint32_t index;
	const static uint32_t BUF_SIZE = 1024;

	inline STDBInfo() {
		bufSize = 0;
		posBuf = NULL;
		index = 0;
	}

	inline ~STDBInfo() {
		reset();
	}

	uint16_t getFileIndex(const string& n);
	void add(POS pos);
	void genDebug(Compiler* compiler);

	bool toFile(int fd);

	bool fromFile(int fd);

	uint32_t getLine(uint32_t index, string& line, string& fname);

	inline void reset() {
		fileTable.clear();

		if(posBuf != NULL) {
			delete []posBuf;
			posBuf = NULL;
		}
		bufSize = 0;
		index = 0;
	}

	inline void clone(STDBInfo* dbg) {
		if(dbg == NULL || index == 0)
			return;
		dbg->reset();
		dbg->index = index;
		dbg->fileTable = fileTable;
		dbg->posBuf = new POS[index];
		memcpy(dbg->posBuf, posBuf, index*sizeof(POS));
	}

	void loadLines(const string& fname, vector<string>& lines);
	void loadFiles();
	const vector<string>* getLines(const string& fname);
} DebugInfo;

class Bytecode {
	PC cindex;
	vector<string> strTable;
	PC *codeBuf;
	uint32_t bufSize;
	Compiler* compiler;

	bool debug;
	DebugInfo debugInfo;

public:
	const static uint32_t BUF_SIZE = 1024;
	Bytecode() {
		debug = false;
		compiler = NULL;
		cindex = 0;
		codeBuf = NULL;
		bufSize = 0;
	}

	inline DebugInfo* getDebug() {
		return &debugInfo;
	}

	string getDebugLine(PC index);

	inline bool isDebug() {
		return debug;
	}

	inline void setCompiler(Compiler* cp) {//for debug
		debug = true;
		compiler = cp;
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
		if(bc == NULL || cindex == 0)
			return;
		bc->reset();
		bc->cindex = cindex;
		bc->strTable = strTable;
		bc->codeBuf = new PC[cindex];
		memcpy(bc->codeBuf, codeBuf, cindex*sizeof(PC));
	}
};

#endif
