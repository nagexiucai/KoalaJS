#ifndef TINYJS_BC_H
#define TINYJS_BC_H
#include <string>
#include <vector>
#include <stack>
#include "Bytecode.h"

typedef enum {
	LEX_EOF = 0,
	LEX_ID = 256,
	LEX_INT,
	LEX_FLOAT,
	LEX_STR,

	LEX_EQUAL,
	LEX_TYPEEQUAL,
	LEX_NEQUAL,
	LEX_NTYPEEQUAL,
	LEX_LEQUAL,
	LEX_LSHIFT,
	LEX_LSHIFTEQUAL,
	LEX_GEQUAL,
	LEX_RSHIFT,
	LEX_RSHIFTUNSIGNED,
	LEX_RSHIFTEQUAL,
	LEX_PLUSEQUAL,
	LEX_MINUSEQUAL,
	LEX_MULTIEQUAL,
	LEX_DIVEQUAL,
	LEX_MODEQUAL,
	LEX_PLUSPLUS,
	LEX_MINUSMINUS,
	LEX_ANDEQUAL,
	LEX_ANDAND,
	LEX_OREQUAL,
	LEX_OROR,
	LEX_XOREQUAL,
	// reserved words
#define LEX_R_LIST_START LEX_R_IF
	LEX_R_IF,
	LEX_R_ELSE,
	LEX_R_DO,
	LEX_R_WHILE,
	LEX_R_FOR,
	LEX_R_BREAK,
	LEX_R_CONTINUE,
	LEX_R_FUNCTION,
	LEX_R_CLASS,
	LEX_R_RETURN,
	LEX_R_VAR,
	LEX_R_CONST,
	LEX_R_TRUE,
	LEX_R_FALSE,
	LEX_R_NULL,
	LEX_R_UNDEFINED,
	LEX_R_NEW,
	LEX_R_INCLUDE,

	LEX_R_LIST_END /* always the last entry */
} LEX_TYPES;

class CScriptLex
{
	public:
		CScriptLex(const std::string &input);
		CScriptLex(CScriptLex *owner, int startChar, int endChar);
		~CScriptLex(void);

		char currCh;
		char nextCh;
		LEX_TYPES tk;                            ///< The type of the token that we have
		int tokenStart;                          ///< Position in the data at the beginning of the token we have here
		int tokenEnd;                            ///< Position in the data at the last character of the token we have here
		int tokenLastEnd;                        ///< Position in the data at the last character of the last token
		std::string tkStr;                           ///< Data contained in the token we have here
		void chkread(int expected_tk);           ///< Lexical match wotsit
		static std::string getTokenStr(int token);   ///< Get the string representation of the given token
		void reset();                            ///< Reset this lex so we can start again

		std::string getSubString(int pos);           ///< Return a sub-string from the given position up until right now
		CScriptLex *getSubLex(int lastPosition); ///< Return a sub-lexer from the given position up until right now

		std::string getPosition(int pos=-1);         ///< Return a string representing the position in lines and columns of the character pos given
		int  dataPos;                            ///< Position in data (we CAN go past the end of the std::string here)

	protected:
		/* When we go into a loop, we use getSubLex to get a lexer for just the sub-part of the
			 relevant string. This doesn't re-allocate and copy the string, but instead copies
			 the data pointer and sets dataOwned to false, and dataStart/dataEnd to the relevant things. */
		char *data;              ///< Data std::string to get tokens from
		int  dataStart, dataEnd; ///< Start and end position in data string
		bool dataOwned;          ///< Do we own this data string?


		void getNextCh();
		void getNextToken();     ///< Get the text token from our text string
};

typedef struct {
	std::vector<PC> breaks;	
	std::vector<PC> continues;	
} Loop;

class Compiler {
	public:

		Compiler();
		~Compiler();

		bool run(const std::string &fname);

		bool exec(const std::string &code);
		
		inline const std::string& getcwd() {
			return cwd;
		}
	
		inline void setcwd(const std::string& cwd) {
			this->cwd = cwd;
		}

		Bytecode bytecode;
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
