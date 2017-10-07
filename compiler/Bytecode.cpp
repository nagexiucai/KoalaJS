#include "Bytecode.h"
#include <unistd.h>  
#include <vector>  
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define INT_SIZE sizeof(uint32_t)
#define INS_SIZE sizeof(uint16_t)

#define P(...) fprintf (stdout, __VA_ARGS__)

void Bytecode::add(PC ins) {
	if(cindex >= bufSize) {
		bufSize = cindex + BUF_SIZE;
		PC *newBuf = new PC[bufSize];

		if(cindex > 0 && codeBuf != NULL) {
			memcpy(newBuf, codeBuf, cindex*sizeof(PC));
			delete []codeBuf;
		}
		codeBuf = newBuf;
	}

	codeBuf[cindex] = ins;
	cindex++;
}
	
string Bytecode::getStr(int i) {
	if(i<0 || i == 0xFFFF ||  i>=strTable.size())
		return "";
	return strTable[i];
}	

uint16_t Bytecode::getStrIndex(const string& n) {
	if(n.length() == 0)
		return 0xFFFF;

	uint16_t sz = strTable.size();
	for(uint16_t i=0; i<sz; ++i) {
		if(strTable[i] == n)
			return i;
	}
	strTable.push_back(n);
	return sz;
}	

PC Bytecode::bytecode(Instr instr, const string& str) {
	Instr r = instr;
	Instr i = 0xFFFF;

	if(str.length() > 0)
		i = this->getStrIndex(str);

	return INS(r, i);
}

void Bytecode::strs() {
	uint16_t sz = strTable.size();
	for(uint16_t i=0; i<sz; ++i) {
		P("%04X: %s\n", i, strTable[i].c_str()); 
	}
}
	
PC Bytecode::gen(Instr instr, const string& str) {
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
	
	PC ins = bytecode(instr, s);
	add(ins);

//	P("%d\t0x%08X\t%s %s\n", cindex-1, ins, BCInstr::instr(instr).c_str(), str.c_str());	

	if(instr == INSTR_INT) {
		add(i);
//		P("%d\t0x%08X\t(%d)\n", cindex-1, i, i);	
	}
	else if(instr == INSTR_FLOAT) {
		memcpy(&i, &f, sizeof(PC));
		add(i);
//		P("%d\t0x%08X\t(%f)\n", cindex-1, i, f);	
	}

	return cindex;
}

/** set jump instructioin , jump to current pc
	@param index, index of instruction.
	@param jmp, jump if true, or njump if false.
	@param back, offset back if true, or forward if false.
 */
void Bytecode::jump(PC anchor, bool jmp, bool back) {
	if(anchor >= cindex) 
		return;

	PC ins = 0;
	if(jmp)
		ins = INS(back? INSTR_JMPB:INSTR_JMP, cindex - anchor);
	else
		ins = INS(back? INSTR_NJMPB:INSTR_NJMP, cindex - anchor);
	if(back)
		add(ins);
	else
		codeBuf[anchor] = ins;
}


void Bytecode::dump() {
	PC i = 0;
	PC ins = 0;

	P("-------string table--------------------\n");
	strs();
	P("---------------------------------------\n");

	while(i < cindex) {
		ins = codeBuf[i];
		Instr instr = (ins >> 16) & 0xFFFF;
		Instr strIndex = ins & 0xFFFF;

		if(strIndex == 0xFFFF)
			P("%d\t0x%08X\t%s\n", i, ins, BCInstr::instr(instr).c_str());	
		else {
			if(instr == INSTR_JMP || instr == INSTR_NJMP || instr == INSTR_NJMPB || instr == INSTR_JMPB)
				P("%d\t0x%08X\t%s %d\n", i, ins, BCInstr::instr(instr).c_str(), strIndex);	
			else
				P("%d\t0x%08X\t%s %s\n", i, ins, BCInstr::instr(instr).c_str(), getStr(strIndex).c_str());	
		}
		
		i++;
	
		if(instr == INSTR_INT) {
			ins = codeBuf[i];
			P("%d\t0x%08X\t(%d)\n", i, ins, ins);	
			i++;
		}
		else if(instr == INSTR_FLOAT) {
			ins = codeBuf[i];
			float f;
			memcpy(&f, &ins, sizeof(PC));
			P("%d\t0x%08X\t(%f)\n", i, ins, f);	
			i++;
		}
	}
}

bool Bytecode::toFile(const std::string& fname) {
	int fd = open(fname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
	if(fd < 0)
		return false;

	// write string talble 
	uint32_t sz = strTable.size();
	for(uint16_t i=0; i<sz; ++i) {
		string s = strTable[i];
		uint32_t len = s.length();
		write(fd, (char*)(&len), INT_SIZE);
		write(fd, s.c_str(), len);
	}	
	sz = 0;	
	write(fd, (char*)(&sz), INT_SIZE);

	// write instruction
	write(fd, (char*)(&cindex), INS_SIZE);
	write(fd, (char*)codeBuf, sizeof(PC) * cindex);

	close(fd);
	return true;
}

bool Bytecode::fromFile(const std::string& fname) {
	reset();

	int fd = open(fname.c_str(), O_RDONLY);
	if(fd < 0)
		return false;
	//read string table

	uint32_t sz;
	char* buf = NULL;

	while(true) {
		read(fd, &sz, INT_SIZE);
		if(sz == 0)
			break;

		buf = new char[sz+1];
		if(buf == NULL)
			return false;
		read(fd, buf, sz);
		buf[sz] = 0;
		
		strTable.push_back(buf);
		delete []buf;
	}	

	read(fd, &cindex, INS_SIZE);
	if(cindex == 0)
		return false;

	sz = cindex * sizeof(PC);
	codeBuf = new PC[sz];
	read(fd, codeBuf, sz);
	
	close(fd);
	return true;
}

