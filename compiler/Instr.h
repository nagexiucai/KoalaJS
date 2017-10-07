#ifndef BC_INSTR
#define BC_INSTR

#include <string>
using namespace std;

typedef unsigned short Instr;
typedef unsigned int PC;

#define INS(ins, off) (((ins) << 16) & 0xFFFF0000 | ((off) & 0x0000FFFF))

const static Instr INSTR_NIL				= 0x0000; // NIL									: Do nothing.

const static Instr INSTR_VAR				= 0x0001; // VAR x								: declare var x
const static Instr INSTR_CONST			= 0x0002; // CONST x							: declare const x
const static Instr INSTR_LOAD				= 0x0003; // LOAD x								: load and push x 
const static Instr INSTR_STORE			= 0x0004; // STORE x							: pop and store to x
const static Instr INSTR_GET				= 0x0005; // : getfield
const static Instr INSTR_ASIGN			= 0x0006; // ASIGN								: =

const static Instr INSTR_INT				= 0x0007; // INT int 							: push int
const static Instr INSTR_FLOAT			= 0x0008; // FLOAT float					: push float 
const static Instr INSTR_STR				= 0x0009; // STR "str"						: push str

const static Instr INSTR_JMP				= 0x000A; // JMP x								: JMP offset x  
const static Instr INSTR_NJMP				= 0x000B; // NJMP x								: Condition not JMP offset x 
const static Instr INSTR_JMPB				= 0x000C; // JMP back x						: JMP back offset x  
const static Instr INSTR_NJMPB			= 0x000D; // NJMP back x					: Condition not JMP back offset x 

const static Instr INSTR_FUNC				= 0x0010; // FUNC x								: function definetion x
const static Instr INSTR_FUNC_END		= 0x0011; // FUNC_END							: function define end
const static Instr INSTR_ARG				= 0x0012; // : function arg
const static Instr INSTR_CALL				= 0x0013; // CALL x								: call function x and push res

const static Instr INSTR_NOT				= 0x0020; // NOT									: !
const static Instr INSTR_MULTI			= 0x0021; // MULTI								: *
const static Instr INSTR_DIV				= 0x0022; // DIV									: /
const static Instr INSTR_MOD				= 0x0023; // MOD									: %
const static Instr INSTR_PLUS				= 0x0024; // PLUS									: + 
const static Instr INSTR_MINUS			= 0x0025; // MINUS								: - 
const static Instr INSTR_NEG				= 0x0026; // NEG									: negate -
const static Instr INSTR_PPLUS			= 0x0027; // PPLUS								: ++
const static Instr INSTR_MMINUS			= 0x0028; // MMINUS								: --
const static Instr INSTR_LSHIFT			= 0x0029; // LSHIFT								: <<
const static Instr INSTR_RSHIFT			= 0x002A; // RSHIFT								: >>
const static Instr INSTR_URSHIFT		= 0x002B; // URSHIFT							: >>>

const static Instr INSTR_EQ					= 0x0030; // EQ										: ==
const static Instr INSTR_NEQ				= 0x0031; // NEQ									: !=
const static Instr INSTR_LEQ				= 0x0032; // LEQ									: <=
const static Instr INSTR_GEQ				= 0x0033; // GEQ									: >=
const static Instr INSTR_GRT				= 0x0034; // GRT									: >
const static Instr INSTR_LES				= 0x0035; // LES									: <

const static Instr INSTR_AAND				= 0x0040; // AAND									: &&
const static Instr INSTR_OOR				= 0x0041; // OOR									: ||
const static Instr INSTR_OR					= 0x0042; // OR										: |
const static Instr INSTR_XOR				= 0x0043; // XOR									: ^
const static Instr INSTR_AND				= 0x0044; // AND									: &


const static Instr INSTR_BREAK			= 0x0050; // : break
const static Instr INSTR_CONTINUE		= 0x0051; // : continue
const static Instr INSTR_RETURN			= 0x0052; // : return

const static Instr INSTR_TRUE				= 0x0060; // : true
const static Instr INSTR_FALSE			= 0x0061; // : false
const static Instr INSTR_NULL				= 0x0062; // : null
const static Instr INSTR_UNDEF			= 0x0063; // : undefined

const static Instr INSTR_NEW				= 0x0070; // : new

class BCInstr {
	public:
	inline static string instr(Instr ins) {
		switch(ins) {
			case  INSTR_NIL					: return "nil";
			case  INSTR_VAR					: return "var";
			case  INSTR_CONST				: return "const";
			case  INSTR_INT					: return "int";
			case  INSTR_FLOAT				: return "float";
			case  INSTR_STR					: return "str";
			case  INSTR_LOAD				: return "load";
			case  INSTR_STORE				: return "store";
			case  INSTR_JMP					: return "jmp";
			case  INSTR_NJMP				: return "njmp";
			case  INSTR_JMPB				: return "jmpb";
			case  INSTR_NJMPB				: return "njmpb";
			case  INSTR_FUNC				: return "func";
			case  INSTR_FUNC_END		: return "func.end";
			case  INSTR_CALL				: return "call";
			case  INSTR_NOT					: return "not";
			case  INSTR_MULTI				: return "multi";
			case  INSTR_DIV					: return "div";
			case  INSTR_MOD					: return "mod";
			case  INSTR_PLUS				: return "plus";
			case  INSTR_MINUS				: return "minus";
			case  INSTR_NEG					: return "neg";
			case  INSTR_PPLUS				: return "pplus";
			case  INSTR_MMINUS			: return "mminus";
			case  INSTR_LSHIFT			: return "lshift";
			case  INSTR_RSHIFT			: return "rshift";
			case  INSTR_URSHIFT			: return "urshift";
			case  INSTR_EQ					: return "eq";
			case  INSTR_NEQ					: return "neq";
			case  INSTR_LEQ					: return "leq";
			case  INSTR_GEQ					: return "geq";
			case  INSTR_GRT					: return "grt";
			case  INSTR_LES					: return "les";
			case  INSTR_AAND				: return "aand";
			case  INSTR_OOR					: return "oor";
			case  INSTR_OR					: return "or";
			case  INSTR_XOR					: return "xor";
			case  INSTR_AND					: return "and";
			case  INSTR_ASIGN				: return "asign";
			case  INSTR_BREAK				: return "break";
			case  INSTR_CONTINUE		: return "continue";
			case  INSTR_RETURN			: return "return";
			case  INSTR_TRUE				: return "true";
			case  INSTR_FALSE				: return "false";
			case  INSTR_NULL				: return "null";
			case  INSTR_UNDEF				: return "undef";
			case  INSTR_ARG					: return "arg";
			case  INSTR_NEW					: return "new";
			case  INSTR_GET					: return "get";
			default									: return "";
		}
	}
};

#endif
