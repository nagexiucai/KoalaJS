#ifndef KOALA_DEBUG_HH
#define KOALA_DEBUG_HH

#ifdef KOALA_DEBUG

#include <string>
#include <vector>
#include "Instr.h"

using namespace std;

class KoalaJS;
class Bytecode;

typedef struct {
	int line;
	string fname;
} Break;

class Debug {
	int fd;
	int lastStop;
	int lastScopeDeep;
	bool next;
	string oldCmd;
	string oldfile;
	vector<Break> breaks;


	void output(const string& s);
	string input();
	bool isBreak(int ln, const string& fname, size_t& index);
	size_t setBreak(Bytecode* bc, const string& cmd, int ln, const string& fname);
	void rmBreak(const string& cmd);
	int list(Bytecode* bc, const string& s, int ln, const string& fname);
	bool getLine(Bytecode* bc, int ln, const string& fname, string& line);
public:
	inline Debug() {
		fd = 0;
		lastStop = -1;
		lastScopeDeep = -1;
		next = true;
	}

	void debug(KoalaJS* js, PC pc);
};

#endif

#endif
