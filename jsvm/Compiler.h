#ifndef TINYJS_BC_H
#define TINYJS_BC_H
#include <string>
#include <vector>
#include <stack>
#include "Bytecode.h"
#include "CScriptLex.h"

typedef struct {
	std::vector<PC> breaks;	
	std::vector<PC> continues;	
} Loop;

class Compiler {
	public:

		Compiler();
		~Compiler();

		bool run(const std::string &fname, Bytecode* bc, bool debug = false);

		bool exec(const std::string &code, Bytecode* bc);
		
		inline const std::string& getcwd() {
			return cwd;
		}
	
		inline void setcwd(const std::string& cwd) {
			this->cwd = cwd;
		}
		
		inline string getFName() {
			return cname;
		}

		inline void getPosition(int *line, int *col, int pos = -1) {
			if(l == NULL)
				return;
			l->getPosition(line, col, pos);
		}

		Bytecode *bytecode;
	private:
		int cindex;
		std::string cwd;
		std::string cname;
		
		std::stack<Loop> loopStack;

		CScriptLex *l;             /// current lexer
	
		LEX_TYPES statement(bool pop = true);

		LEX_TYPES block();

		LEX_TYPES base();

		LEX_TYPES defFunc(string& name);

		LEX_TYPES defClass();

		LEX_TYPES callFunc(int &argNum);

		LEX_TYPES factor();

		LEX_TYPES unary();

		LEX_TYPES term();

		LEX_TYPES expr();

		LEX_TYPES shift();

		LEX_TYPES condition();

		LEX_TYPES logic();

		LEX_TYPES ternary();

		Loop* getLoop();

		void setLoopBreaks(Loop* loop, PC pc);

		void setLoopContinues(Loop* loop, PC pc);
};

#endif
