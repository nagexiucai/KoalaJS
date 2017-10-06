#include "Out.h"

#define OUT(...) fprintf (stdout, __VA_ARGS__)

unsigned int BCOut::bytecode(Instr instr, const string& str) {
	unsigned r = instr;
	unsigned short i = 0;

	if(str.length() > 0)
	 	i = this->str(str);

	return ((r << 16) & 0xFFFF0000) | (i & 0x0000FFFF);
}

void BCOut::strs() {
	OUT("---------------------------\n");
	unsigned short sz = strTable.size();
	for(unsigned short i=0; i<sz; ++i) {
		OUT("\t%04X: %s\n", i, strTable[i].c_str()); 
	}
}

void BCOut::out(Instr instr, const string& str) {
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

	if(s.length() == 0)
		OUT("%d\t 0x%08X %s\n", cindex++, ins, BCInstr::instr(instr).c_str());	
	else
		OUT("%d\t 0x%08X %s %s\n", cindex++, ins, BCInstr::instr(instr).c_str(), str.c_str());	

	if(instr == INSTR_INT) {
		OUT("%d\t 0x%08X (%d)\n", cindex++, i, i);	
	}
	else if(instr == INSTR_FLOAT) {
		memcpy(&i, &f, sizeof(unsigned int));
		OUT("%d\t 0x%08X (%f)\n", cindex++, i, f);	
	}
}

