#include "Bytecode.h"
#include "Compiler.h"
#include <unistd.h>  
#include <vector>  
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define INT_SIZE sizeof(uint32_t)
#define INS_SIZE sizeof(uint16_t)

#define P(...) fprintf (stdout, __VA_ARGS__)

void DebugInfo::add(POS pos) {
	if(index >= bufSize) {
		bufSize = index + BUF_SIZE;
		POS *newBuf = new POS[bufSize];

		if(index > 0 && posBuf != NULL) {
			memcpy(newBuf, posBuf, index*sizeof(POS));
			delete []posBuf;
		}
		posBuf = newBuf;
	}

	posBuf[index] = pos;
	index++;
}

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
	if(i<0 || i == 0xFFFF ||  i>=(int)strTable.size())
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

PC Bytecode::bytecode(OpCode instr, const string& str) {
	OpCode r = instr;
	OpCode i = 0xFFFF;

	if(str.length() > 0)
		i = this->getStrIndex(str);

	return INS(r, i);
}
		
PC Bytecode::gen(OpCode instr, const string& str) {
	int i = 0;
	float f = 0.0;
	string s = str;

	if(instr == INSTR_INT) {
		if(str.find("0x") != string::npos ||
				str.find("0x") != string::npos) 
			i = strtol(str.c_str(), NULL, 16);
		else
			i = strtol(str.c_str(), NULL, 10);
		s = "";
	}
	else if(instr == INSTR_FLOAT) {
		f = atof(str.c_str());
		s = "";
	}
	
	PC ins = bytecode(instr, s);
	add(ins);

//	P("%d\t0x%08X\t%s %s\n", cindex-1, ins, BCOpCode::instr(instr).c_str(), str.c_str());	

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

PC Bytecode::gen(OpCode instr, int i) {
	PC ins = bytecode(instr, "");
	add(ins);
	add(i);
	return cindex;
}

void Bytecode::setJmp(PC anchor, OpCode op, PC target) {
	if(target == 0xFFFFFFFF)
		target = cindex;

	int offset = target > anchor ? (target-anchor) : (anchor-target);
	PC ins = INS(op, offset);
	codeBuf[anchor] = ins;
}

void Bytecode::addJmp(PC anchor, OpCode op, PC target) {
	if(target == 0xFFFFFFFF)
		target = cindex;

	int offset = target > anchor ? (target-anchor) : (anchor-target);
	PC ins = INS(op, offset);
	add(ins);
}

string Bytecode::strs() {
	string ret;
	char index[8];
	uint16_t sz = strTable.size();
	for(uint16_t i=0; i<sz; ++i) {
		sprintf(index, "%04X: ", i);
		ret = ret + index + strTable[i] + "\n";
	}
	return ret;
}

string Bytecode::dump() {
	string ret;
	char s[32];
	PC i = 0;
	PC ins = 0;

	ret = "-------string table--------------------\n";
	ret += strs();
	ret += "---------------------------------------\n";

	while(i < cindex) {
		ins = codeBuf[i];
		OpCode instr = (ins >> 16) & 0xFFFF;
		OpCode strIndex = ins & 0xFFFF;

		if(strIndex == 0xFFFF) {
			sprintf(s, "%04d 0x%08X ; %s\n", i, ins, BCOpCode::instr(instr).c_str());	
			ret += s;
		}
		else {
			if(instr == INSTR_JMP || 
					instr == INSTR_NJMP || 
					instr == INSTR_NJMPB ||
					instr == INSTR_JMPB) {
				sprintf(s, "%04d 0x%08X ; %s %d\n", i, ins, BCOpCode::instr(instr).c_str(), strIndex);	
				ret += s;
			}
			else if(instr == INSTR_STR) {
				sprintf(s, "%04d 0x%08X ; %s \"", i, ins, BCOpCode::instr(instr).c_str());	
				ret = ret + s + getStr(strIndex) + "\"\n";	
			}
			else {
				sprintf(s, "%04d 0x%08X ; %s ", i, ins, BCOpCode::instr(instr).c_str());	
				ret = ret + s + getStr(strIndex) + "\n";	
			}
		}
		
		i++;
	
		if(instr == INSTR_INT) {
			ins = codeBuf[i];
			sprintf(s, "%04d 0x%08X ; (%d)\n", i, ins, ins);	
			ret += s;
			i++;
		}
		else if(instr == INSTR_FLOAT) {
			ins = codeBuf[i];
			float f;
			memcpy(&f, &ins, sizeof(PC));
			sprintf(s, "%04d 0x%08X ; (%f)\n", i, ins, f);	
			ret += s;
			i++;
		}
	}
	return ret;
}

bool Bytecode::toFile(const std::string& fname) {
	int fd = open(fname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP);
	if(fd < 0)
		return false;

	// write string talble 
	uint32_t sz = (uint32_t)strTable.size();
	for(uint16_t i=0; i<sz; ++i) {
		string s = strTable[i];
		uint32_t len = (uint32_t)s.length();
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

