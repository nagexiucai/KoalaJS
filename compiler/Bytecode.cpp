#include "Bytecode.h"

#define P(...) fprintf (stdout, __VA_ARGS__)

void Bytecode::add(unsigned int ins) {
	if(cindex >= bufSize) {
		bufSize = cindex + BUF_SIZE;
		unsigned int *newBuf = new unsigned int[bufSize];

		if(cindex > 0 && codeBuf != NULL) {
			memcpy(newBuf, codeBuf, cindex*sizeof(unsigned int));
			delete []codeBuf;
		}
		codeBuf = newBuf;
	}

	codeBuf[cindex] = ins;
	cindex++;
}

unsigned int Bytecode::bytecode(Instr instr, const string& str) {
	unsigned r = instr;
	unsigned short i = 0xFFFF;

	if(str.length() > 0)
	 	i = this->getStrIndex(str);

	return ((r << 16) & 0xFFFF0000) | (i & 0x0000FFFF);
}

void Bytecode::strs() {
	unsigned short sz = strTable.size();
	for(unsigned short i=0; i<sz; ++i) {
		P("%04X: %s\n", i, strTable[i].c_str()); 
	}
}

void Bytecode::gen(Instr instr, const string& str) {
	int i = 0;
	float f = 0.0;
	string s = str;

	if(instr == INSTR_INT) {
		i = atoi(str.c_str());
		s = "";
	}
	else if(instr == INSTR_FLOAT) {
		f = atof(str.c_str());
		s = "";
	}
	
	unsigned int ins = bytecode(instr, s);
	add(ins);

	if(s.length() == 0) {
		//P("%d\t0x%08X\t%s\n", cindex-1, ins, BCInstr::instr(instr).c_str());	
	}
	else {
		//P("%d\t0x%08X\t%s %s\n", cindex-1, ins, BCInstr::instr(instr).c_str(), str.c_str());	
	}

	if(instr == INSTR_INT) {
		add(i);
		//P("%d\t0x%08X\t(%d)\n", cindex-1, i, i);	
	}
	else if(instr == INSTR_FLOAT) {
		memcpy(&i, &f, sizeof(unsigned int));
		add(i);
		//P("%d\t0x%08X\t(%f)\n", cindex-1, i, f);	
	}
}

void Bytecode::dump() {
	unsigned int i = 0;
	unsigned int ins = 0;

	P("-------string table--------------------\n");
	strs();
	P("---------------------------------------\n");

	while(i < cindex) {
		ins = codeBuf[i];
		unsigned short instr = (ins >> 16) & 0xFFFF;
		unsigned short strIndex = ins & 0xFFFF;

		if(strIndex == 0xFFFF)
			P("%d\t0x%08X\t%s\n", i, ins, BCInstr::instr(instr).c_str());	
		else
			P("%d\t0x%08X\t%s %s\n", i, ins, BCInstr::instr(instr).c_str(), getStr(strIndex).c_str());	
		
		i++;
	
		if(instr == INSTR_INT) {
			ins = codeBuf[i];
			P("%d\t0x%08X\t(%d)\n", i, ins, ins);	
			i++;
		}
		else if(instr == INSTR_FLOAT) {
			ins = codeBuf[i];
			float f;
			memcpy(&f, &ins, sizeof(unsigned int));
			P("%d\t0x%08X\t(%f)\n", i, ins, f);	
			i++;
		}
	}
}
