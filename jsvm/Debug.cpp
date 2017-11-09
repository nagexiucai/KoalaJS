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

bool Debug::isBreak(int line, const string& fname, size_t &index) {
	index = 0;
	if(oldLine >= 0) {
		if(oldLine != line)
			return true;
		else
			return false;
	}

	size_t sz = breaks.size();
	for(size_t i=0; i<sz; ++i) {
		if(line == breaks[i].line && fname == breaks[i].fname) {
			index = i+1;
			return true;
		}
	}
	return false;
}

size_t Debug::setBreak(const string& cmd, int ln, const string& fname) {
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

void Debug::debug(KoalaJS* js, PC pc) {
	Bytecode* bc = js->getBytecode();
	if(bc == NULL)
		return;

	string sline, fname;	
	int ln = bc->getDebug()->getLine(pc, sline, fname);
	size_t index = 0;
	if(pc > 0 && !isBreak(ln, fname, index)) 
		return;
	
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

	while(true) {
		string s = input();
		s = StringUtil::trim(s);
		if(s.length() == 0)
			s = oldCmd;
		else
			oldCmd = s;

		if(s.length() == 0)
			continue;

		if(s.substr(0, 5) == "conti" || s == "c") {
			oldLine = -1;
			break;
		} 
		else if(s == "quit" || s == "q") {
			js->stop();
			break;
		}
		else if(s == "next" || s == "n") {
			oldLine = ln;
			break;
		}
		else if(s.substr(0, 5) == "break" || s == "b" || s.substr(0, 2) == "b ") {
			setBreak(s, ln, fname);
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
