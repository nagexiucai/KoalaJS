#ifndef KOALA_DEBUG
#define KOALA_DEBUG

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
	int oldLine;
	string oldCmd;
	string oldfile;
	vector<Break> breaks;


	void output(const string& s);
	string input();
	bool isBreak(int ln, const string& fname, size_t& index);
	size_t setBreak(const string& s, int ln, const string& fname);
public:
	inline Debug() {
		fd = 0;
		oldLine = -1;
	}

	void debug(KoalaJS* js, PC pc);
};

#endif
