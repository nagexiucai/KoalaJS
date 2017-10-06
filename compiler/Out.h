#ifndef BC_OUT_HH
#define BC_OUT_HH

#include <string>
using namespace std;

typedef unsigned short Instr;

const static Instr INSTR_VAR			= 0x0001; // VAR x								: declare var x
const static Instr INSTR_CONST		= 0x0002; // CONST x							: declare const x
const static Instr INSTR_INT			= 0x0003; // INT int 							: push int
const static Instr INSTR_FLOAT		= 0x0004; // FLOAT float					: push float 
const static Instr INSTR_STR			= 0x0005; // STR "str"						: push str
const static Instr INSTR_LOAD			= 0x0006; // LOAD x								: load and push x 
const static Instr INSTR_STORE		= 0x0007; // STORE x							: pop and store to x
const static Instr INSTR_FUNC			= 0x0008; // FUNC x								: function definetion x
const static Instr INSTR_FUNC_END = 0x0009; // FUNC_END							: function define end
const static Instr INSTR_CALL			= 0x000A; // CALL x								: call function x and push res

const static Instr INSTR_NOT			= 0x000B; // NOT									: !
const static Instr INSTR_MULTI		= 0x000C; // MULTI								: *
const static Instr INSTR_DIV			= 0x000D; // DIV									: /
const static Instr INSTR_MOD			= 0x000E; // MOD									: %
const static Instr INSTR_PLUS			= 0x000F; // PLUS									: + 
const static Instr INSTR_MINUS		= 0x0010; // MINUS								: - 
const static Instr INSTR_NEG			= 0x0011; // NEG									: negate -
const static Instr INSTR_PPLUS		= 0x0012; // PPLUS								: ++
const static Instr INSTR_MMINUS		= 0x0013; // MMINUS								: --
const static Instr INSTR_LSHIFT		= 0x0014; // LSHIFT								: <<
const static Instr INSTR_RSHIFT		= 0x0015; // RSHIFT								: >>
const static Instr INSTR_URSHIFT	= 0x0016; // URSHIFT							: >>>

const static Instr INSTR_EQ				= 0x0017; // EQ										: ==
const static Instr INSTR_NEQ			= 0x0018; // NEQ									: !=
const static Instr INSTR_LEQ			= 0x0019; // LEQ									: <=
const static Instr INSTR_GEQ			= 0x001A; // GEQ									: >=
const static Instr INSTR_GRT			= 0x001B; // GRT									: >
const static Instr INSTR_LES			= 0x001C; // LES									: <

const static Instr INSTR_AAND			= 0x001D; // AAND									: &&
const static Instr INSTR_OOR			= 0x001E; // OOR									: ||
const static Instr INSTR_OR				= 0x001F; // OR										: |
const static Instr INSTR_XOR			= 0x0020; // XOR									: ^
const static Instr INSTR_AND			= 0x0021; // AND									: &

const static Instr INSTR_ASIGN		= 0x0022; // ASIGN								: =

const static Instr INSTR_BREAK			= 0x0023; // : break
const static Instr INSTR_CONTINUE		= 0x0024; // : continue
const static Instr INSTR_RETURN			= 0x0025; // : return
const static Instr INSTR_WHILE			= 0x0026; // : while
const static Instr INSTR_WHILE_END	= 0x0027; // : while end
const static Instr INSTR_BLOCK			= 0x0028; // : block
const static Instr INSTR_BLOCK_END	= 0x0029; // : block end
const static Instr INSTR_IF					= 0x002A; // : if
const static Instr INSTR_IF_END			= 0x002B; // : if end
const static Instr INSTR_ELSE				= 0x002C; // : else
const static Instr INSTR_ELSE_END		= 0x002D; // : else end
const static Instr INSTR_CONDI			= 0x002E; // : condition
const static Instr INSTR_CONDI_END	= 0x002F; // : condition end

const static Instr INSTR_TRUE				= 0x0030; // : true
const static Instr INSTR_FALSE			= 0x0031; // : false
const static Instr INSTR_NULL				= 0x0032; // : null
const static Instr INSTR_UNDEF			= 0x0033; // : undefined

const static Instr INSTR_ARG				= 0x0034; // : function arg
const static Instr INSTR_NEW				= 0x0035; // : new
const static Instr INSTR_GET				= 0x0036; // : getfield

class BCOut {
public:

	void out(unsigned short instr, const string& str = "");
};

#endif
