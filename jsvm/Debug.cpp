#include "Debug.h"
#include "utils/String/StringUtil.h"
#include "KoalaJS.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
 
void Debug::output(const string& s) {
	write(fd, s.c_str(), s.length());
	fsync(fd);
}

string Debug::input() {
	output("(jsdb) ");
	char input[1024];
	int sz = read(fd, input, 1023);
	if(sz >= 0)
		input[sz] = 0;
	return input;
}

void Debug::rmBreak(const string& cmd) {
	int index = -1;
	string s = cmd;
	if(s.find("rm ") != string::npos) {
		s = StringUtil::trim(s.substr(3));
		if(s.length() > 0)
			index = atoi(s.c_str()) - 1;
	}

	if(index <0  || index >= (int)breaks.size()) {
		return;
	}
	breaks.erase(breaks.begin() + index);
}

bool Debug::isBreak(int line, const string& fname, size_t &index) {
	index = 0;
	size_t sz = breaks.size();
	for(size_t i=0; i<sz; ++i) {
		if(line == breaks[i].line && fname == breaks[i].fname) {
			index = i+1;
			return true;
		}
	}
	return false;
}

size_t Debug::setBreak(Bytecode* bc, const string& cmd, int ln, const string& fname) {
	string s = cmd;
	if(s.find("break ") != string::npos) {
		s = StringUtil::trim(s.substr(6));
		if(s.length() > 0)
			ln = atoi(s.c_str());
	}
	else if(s.find("b ") != string::npos) {
		s = StringUtil::trim(s.substr(2));
		if(s.length() > 0)
			ln = atoi(s.c_str());
	}

	while(true) {
		if(!getLine(bc, ln, fname, s))
			return 0;
		s = StringUtil::trim(s);
		if(s.length() > 0)
			break;
		ln++;
	}

	size_t index;
	if(!isBreak(ln, fname, index)) {
		Break b;
		b.line = ln;
		b.fname = fname;
		breaks.push_back(b);
		index = breaks.size();
	}
	return index;
}

bool Debug::getLine(Bytecode* bc, int ln, const string& fname, string& line) {
	const vector<string>* lines = bc->getDebug()->getLines(fname);
	if(lines == NULL)
		return false;

	int sz = (int)lines->size();
	if(ln <= 0 || ln > sz)
		return false;

	line = (*lines)[ln-1];
	return true;
}

#define LIST_LINES 8
int Debug::list(Bytecode* bc, const string& cmd, int ln, const string& fname) {
	const vector<string>* lines = bc->getDebug()->getLines(fname);
	if(lines == NULL)
		return 0;

	string s = cmd;
	bool sign = false;
	if(s.find("list ") != string::npos) {
		s = StringUtil::trim(s.substr(5));
		if(s.length() > 0)
			ln = atoi(s.c_str());
		sign = true;
	}
	else if(s.find("l ") != string::npos) {
		s = StringUtil::trim(s.substr(2));
		if(s.length() > 0)
			ln = atoi(s.c_str());
		sign = true;
	}
	
	int sz = (int)lines->size();
	int low = ln - 8;
	if(low < 0) 
		low = 0;
	int high = ln + 8;
	if(high >= sz)
		high = sz;

	for(int i=low; i<high; ++i) {
		if(sign && (i+1) == ln)
			output(StringUtil::from(i+1) + ":==> " + (*lines)[i] + "\n");
		else
			output(StringUtil::from(i+1) + ":    " + (*lines)[i] + "\n");
	}
	return ln + LIST_LINES*2;
}

void Debug::debug(KoalaJS* js, PC pc) {
	Bytecode* bc = js->getBytecode();
	if(bc == NULL)
		return;

	string sline, fname;	
	int ln = bc->getDebug()->getLine(pc, sline, fname);
	size_t index = 0;

	if(lastStop == ln || 
				(lastScopeDeep >= 0 && lastScopeDeep < js->scopeDeep())) //dont block at same line.
		return;
	if(!next && !isBreak(ln, fname, index)) 
		return;
	lastScopeDeep = js->scopeDeep();
	lastStop = ln;
	if(oldfile != fname || index > 0) {
		output("\nbreakpoint ");
		output(StringUtil::from((int)index) + ", at " +
				fname + "\n");
		oldfile = fname;
	}
	else { 
		output("\n");
	}
	output(StringUtil::from(ln) + 
				":    " + sline + "\n");

	int lline = -1; //list pos
	while(true) {
		string s = input();
		s = StringUtil::trim(s);
		if(s.length() == 0)
			s = oldCmd;
		else
			oldCmd = s;

		if(s.length() == 0)
			continue;

		if(s.substr(0, 4) == "cont" || s == "c") {
			next = false;
			break;
		} 
		else if(s == "quit" || s == "q") {
			js->stop();
			break;
		}
		else if(s == "next" || s == "n") {
			next = true;
			break;
		}
		else if(s.substr(0, 5) == "break" || s == "b" || s.substr(0, 2) == "b ") {
			setBreak(bc, s, ln, fname);
		}
		else if(s.substr(0, 3) == "rm ") {
			rmBreak(s);
		}
		else if(s.substr(0, 4) == "list" || s == "l" || s.substr(0, 2) == "l ") {
			if(lline < 0)
				lline = ln;
			lline = list(bc, s, lline, fname);
		}
		else if(s == "dump" || s == "d") {
			output(js->getRoot()->getString() + "\n");
		}
		else if(s.find("print ") != string::npos) {
			s = StringUtil::trim(s.substr(6));
			if(s.length() > 0) {
				BCNode* n = js->load(s);
				if(n != NULL) {
					s = n->var->getString();
					output(s + "\n");
				}
			}
		}
		else if(s == "help" || s == "h") {
			output("commands:\n  [q]uit\n  [p]rint\n  [d]ump\n  [c]ontinue\n  [b]reak\n  [r]m\n  [h]elp\n");
		}
		else {
			string msg = "Unknown command: ";
			msg = msg + s + "\n";
			output(msg);
		}
	}
}
