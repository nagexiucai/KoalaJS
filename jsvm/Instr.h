#ifndef BC_INSTR
#define BC_INSTR

#include <string>
#include <stdint.h>

using namespace std;

typedef uint16_t OprCode;
typedef uint32_t PC; //PC for : Program Counter

#define INS(ins, off) ((((ins) << 16) & 0xFFFF0000) | ((off) & 0x0000FFFF))

const static OprCode INSTR_NIL					= 0x0000; // NIL									: Do nothing.

const static OprCode INSTR_VAR					= 0x0001; // VAR x								: declare var x
const static OprCode INSTR_CONST				= 0x0002; // CONST x							: declare const x
const static OprCode INSTR_LOAD				= 0x0003; // LOAD x								: load and push x 
const static OprCode INSTR_STORE				= 0x0004; // STORE x							: pop and store to x
const static OprCode INSTR_GET					= 0x0005; // getfield
const static OprCode INSTR_ASIGN				= 0x0006; // ASIGN								: =

const static OprCode INSTR_INT					= 0x0007; // INT int 							: push int
const static OprCode INSTR_FLOAT				= 0x0008; // FLOAT float					: push float 
const static OprCode INSTR_STR					= 0x0009; // STR "str"						: push str
const static OprCode INSTR_ARRAY_AT		= 0x000A; // ARRAT 								: get array element at
const static OprCode INSTR_ARRAY				= 0x000B; // ARRAY 								: array start
const static OprCode INSTR_ARRAY_END		= 0x000C; // ARRAY_END 						: array end

const static OprCode INSTR_FUNC				= 0x0010; // FUNC x								: function definetion x
const static OprCode INSTR_FUNC_GET		= 0x0011; // GET FUNC x						: class get function definetion x
const static OprCode INSTR_FUNC_SET		= 0x0012; // SET FUNC x						: class set function definetion x
const static OprCode INSTR_CALL				= 0x0013; // CALL x								: call function x and push res
const static OprCode INSTR_CALLO				= 0x0014; // CALL obj.x						: call object member function x and push res
const static OprCode INSTR_CLASS				= 0x0015; // class								: define class
const static OprCode INSTR_CLASS_END		= 0x0016; // class end						: end of class definition
const static OprCode INSTR_MEMBER			= 0x0017; // member without name
const static OprCode INSTR_MEMBERN			= 0x0018; // : member with name

const static OprCode INSTR_NOT					= 0x0020; // NOT									: !
const static OprCode INSTR_MULTI				= 0x0021; // MULTI								: *
const static OprCode INSTR_DIV					= 0x0022; // DIV									: /
const static OprCode INSTR_MOD					= 0x0023; // MOD									: %
const static OprCode INSTR_PLUS				= 0x0024; // PLUS									: + 
const static OprCode INSTR_MINUS				= 0x0025; // MINUS								: - 
const static OprCode INSTR_NEG					= 0x0026; // NEG									: negate -
const static OprCode INSTR_PPLUS				= 0x0027; // PPLUS								: x++
const static OprCode INSTR_MMINUS			= 0x0028; // MMINUS								: x--
const static OprCode INSTR_PPLUS_PRE		= 0x0029; // PPLUS								: ++x
const static OprCode INSTR_MMINUS_PRE	= 0x002A; // MMINUS								: --x
const static OprCode INSTR_LSHIFT			= 0x002B; // LSHIFT								: <<
const static OprCode INSTR_RSHIFT			= 0x002C; // RSHIFT								: >>
const static OprCode INSTR_URSHIFT			= 0x002D; // URSHIFT							: >>>

const static OprCode INSTR_EQ					= 0x0030; // EQ										: ==
const static OprCode INSTR_NEQ					= 0x0031; // NEQ									: !=
const static OprCode INSTR_LEQ					= 0x0032; // LEQ									: <=
const static OprCode INSTR_GEQ					= 0x0033; // GEQ									: >=
const static OprCode INSTR_GRT					= 0x0034; // GRT									: >
const static OprCode INSTR_LES					= 0x0035; // LES									: <

const static OprCode INSTR_PLUSEQ			= 0x0036; // +=
const static OprCode INSTR_MINUSEQ			= 0x0037; // -=	
const static OprCode INSTR_MULTIEQ			= 0x0038; // *=
const static OprCode INSTR_DIVEQ				= 0x0039; // /=
const static OprCode INSTR_MODEQ				= 0x003A; // %=

const static OprCode INSTR_AAND				= 0x0040; // AAND									: &&
const static OprCode INSTR_OOR					= 0x0041; // OOR									: ||
const static OprCode INSTR_OR					= 0x0042; // OR										: |
const static OprCode INSTR_XOR					= 0x0043; // XOR									: ^
const static OprCode INSTR_AND					= 0x0044; // AND									: &

const static OprCode INSTR_BREAK				= 0x0050; // : break
const static OprCode INSTR_CONTINUE		= 0x0051; // : continue
const static OprCode INSTR_RETURN			= 0x0052; // : return none value
const static OprCode INSTR_RETURNV			= 0x0053; // : return with value

const static OprCode INSTR_NJMP				= 0x0054; // NJMP x								: Condition not JMP offset x 
const static OprCode INSTR_JMPB				= 0x0055; // JMP back x						: JMP back offset x  
const static OprCode INSTR_NJMPB				= 0x0056; // NJMP back x					: Condition not JMP back offset x 
const static OprCode INSTR_JMP					= 0x0057; // JMP x								: JMP offset x  

const static OprCode INSTR_TRUE				= 0x0060; // : true
const static OprCode INSTR_FALSE				= 0x0061; // : false
const static OprCode INSTR_NULL				= 0x0062; // : null
const static OprCode INSTR_UNDEF				= 0x0063; // : undefined

const static OprCode INSTR_NEW					= 0x0070; // : new

const static OprCode INSTR_POP					= 0x0080; // : pop and release

const static OprCode INSTR_OBJ					= 0x0090; // : object for JSON 
const static OprCode INSTR_OBJ_END			= 0x0091; // : object end for JSON 

const static OprCode INSTR_INCLUDE			= 0x00A0; // : include 

class BCOprCode {
	public:
	inline static string instr(OprCode ins) {
		switch(ins) {
			case  INSTR_NIL					: return "nil";
			case  INSTR_OBJ					: return "obj";
			case  INSTR_OBJ_END			: return "obje";
			case  INSTR_MEMBER			: return "member";
			case  INSTR_MEMBERN			: return "membern";
			case  INSTR_POP					: return "pop";
			case  INSTR_VAR					: return "var";
			case  INSTR_CONST				: return "const";
			case  INSTR_INT					: return "int";
			case  INSTR_FLOAT				: return "float";
			case  INSTR_STR					: return "str";
			case  INSTR_ARRAY_AT		: return "arrat";
			case  INSTR_ARRAY				: return "arr";
			case  INSTR_ARRAY_END		: return "arre";
			case  INSTR_LOAD				: return "load";
			case  INSTR_STORE				: return "store";
			case  INSTR_JMP					: return "jmp";
			case  INSTR_NJMP				: return "njmp";
			case  INSTR_JMPB				: return "jmpb";
			case  INSTR_NJMPB				: return "njmpb";
			case  INSTR_FUNC				: return "func";
			case  INSTR_FUNC_GET		: return "funcget";
			case  INSTR_FUNC_SET		: return "funcset";
			case  INSTR_CLASS				: return "class";
			case  INSTR_CLASS_END		: return "classe";
			case  INSTR_CALL				: return "call";
			case  INSTR_CALLO				: return "callo";
			case  INSTR_NOT					: return "not";
			case  INSTR_MULTI				: return "multi";
			case  INSTR_DIV					: return "div";
			case  INSTR_MOD					: return "mod";
			case  INSTR_PLUS				: return "plus";
			case  INSTR_MINUS				: return "minus";
			case  INSTR_NEG					: return "neg";
			case  INSTR_PPLUS				: return "pplus";
			case  INSTR_MMINUS			: return "mminus";
			case  INSTR_PPLUS_PRE		: return "pplusp";
			case  INSTR_MMINUS_PRE	: return "mminusp";
			case  INSTR_LSHIFT			: return "lshift";
			case  INSTR_RSHIFT			: return "rshift";
			case  INSTR_URSHIFT			: return "urshift";
			case  INSTR_EQ					: return "eq";
			case  INSTR_NEQ					: return "neq";
			case  INSTR_LEQ					: return "leq";
			case  INSTR_GEQ					: return "geq";
			case  INSTR_GRT					: return "grt";
			case  INSTR_LES					: return "les";
			case  INSTR_PLUSEQ			: return "pluseq";
			case  INSTR_MINUSEQ			: return "minuseq";
			case  INSTR_MULTIEQ			: return "multieq";
			case  INSTR_DIVEQ				: return "diveq";
			case  INSTR_MODEQ				: return "modeq";
			case  INSTR_AAND				: return "aand";
			case  INSTR_OOR					: return "oor";
			case  INSTR_OR					: return "or";
			case  INSTR_XOR					: return "xor";
			case  INSTR_AND					: return "and";
			case  INSTR_ASIGN				: return "asign";
			case  INSTR_BREAK				: return "break";
			case  INSTR_CONTINUE		: return "continue";
			case  INSTR_RETURN			: return "return";
			case  INSTR_RETURNV			: return "returnv";
			case  INSTR_TRUE				: return "true";
			case  INSTR_FALSE				: return "false";
			case  INSTR_NULL				: return "null";
			case  INSTR_UNDEF				: return "undef";
			case  INSTR_NEW					: return "new";
			case  INSTR_GET					: return "get";
			case  INSTR_INCLUDE			: return "include";
			default									: return "";
		}
	}
};

#endif
