#ifndef _SGSCRIPTOBJ_H_
#define _SGSCRIPTOBJ_H_

/*
	Stmts 	-> 	Stmt Stmts | eps
	Stmt 	->	func ( Stmts )
		|	proc
		|	value
		|	var
*/

#include "Global.h"
#include "SymbolTable.h"

#include "m_pd.h"

#define MAX_PROGRAM_LENGTH 256
#define OUTPUTBUFFER_LENGTH 256

DECL_LIST(ListAtom,ElementAtom,t_atom,copybytes,getbytes,freebytes);


//typedef ListAtom Code;
typedef t_atom Command;
typedef Command* Code;
struct SFunctionInfo;
#define FunctionInfo struct SFunctionInfo

// information kept during runtime for each function:
typedef struct SCommandInfo {
	t_int stackHeight0;
	FunctionInfo* pFunctionInfo;
} CommandInfo;
DECL_LIST(ListCommand,ElementCommand,CommandInfo,copybytes,getbytes,freebytes);

typedef struct _sgScript {
	//internal obj information:
	t_object obj;
	t_outlet* pOutlet;
	// skip mode (default FALSE):
	BOOL skipMode;
	//symbol table
	SymbolTable* pSymbolTable;
	// return stack
	ListAtom stack;
	// CommandStack:
	ListCommand cmdStack;
	// current program:
	t_int currentProgCount;
	Code currentCode;
	// code
	t_int cmdCount;
	Code code;
	// code
	t_int metaCodeCount;
	Code metaCode;
	// instruction pointer
	t_int peek;
	// output buffer:
	t_int outputBufferCount;
	t_atom outputBuffer[OUTPUTBUFFER_LENGTH];
} t_sgScript;


t_class* sgScriptObjInit();
void sgScriptObjExit();


CONSTRUCTOR(sgScript,);
DESTRUCTOR(sgScript,pThis);

void sgScriptObj_OnExecute(t_sgScript* pThis);
void sgScriptObj_OnExecMeta(t_sgScript* pThis, t_symbol* s, int argc, t_atom* argv);
void sgScriptObj_OnSetProgram(t_sgScript* pThis, t_symbol* s, int argc, t_atom* argv);
void sgScriptObj_OnSetVar(t_sgScript* pThis, t_symbol* s, int argc, t_atom* argv);

// executes the current code:
void sgScriptObj_exec(t_sgScript* pThis);

// lexer methods:
typedef int POS;
#define END_OF_FILE -1
#define ERROR -2
typedef BOOL TokenFits;
t_atom* lexer_getNextToken(t_sgScript* pThis);
TokenFits lexer_consumeNextToken(t_sgScript* pThis, t_atom* pExpectedSym);
void lexer_setIP(t_sgScript* pThis, t_int peek);


void freeProgram(t_sgScript* pThis);
void freeMetaProgram(t_sgScript* pThis);

typedef void (*POperatorFunction) (t_sgScript* pThis, t_int countParam, t_atom* pArgs);

// describes the restrictions for a function:
/*
  Please notice:
  if executeAfter != -1, the function may be executed BEFORE its closing ")".
  it is therefore important, that such a function either jumps after after its corresponding ")" via "jump", or explicitely puts a new command on the command stack!
*/
struct SFunctionInfo {
	t_atom name;
	t_int paramCount;
	// -1 means variable

	t_int executeAfter;
	/* if != -1 this means the function is called after "executeAfter" parameters,
		just ignoring the parameters that might follow
	  -1 is the usual behaviour. the function is called when all parameters have
	  	been parsed, that means when the ')' is found.
	*/
	POperatorFunction pFunc;
};
#define FUNCTIONINFO(VARNAME,NAME,PARAMCOUNT,EXECAFTER,PFUNC)\
	FunctionInfo VARNAME;\
	VARNAME . name = NAME;\
	VARNAME . paramCount = PARAMCOUNT ;\
	VARNAME . executeAfter = EXECAFTER ;\
	VARNAME . pFunc = PFUNC

//BOOL isFunction(t_atom* pToken);
FunctionInfo* getFunctionInfo(t_atom* pToken);
BOOL isValue(t_atom* pToken);
/*
  checks if the next command is one that can be executed before all parameters have been read. If this is the case, and if there are enough parameters on stack for it, executed it.
*/
void tryToExecuteImmediately(t_sgScript* pThis);
//BOOL isVar(t_atom* pToken);
//BOOL isProc(t_atom* pToken);

void add(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void sub(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void mul(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void div(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void mod(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
// output:
void print(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void pack(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void out(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
// variables:
void addVar(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void getVar(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void getVarA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void setVar(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void setVarA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
// main variables:
void addMainVar(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void clearMain(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
// conditionality:
void if_(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
// sgScales:
void sgFunc(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void sgScale(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
// boolean operators:
void and_(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void or_(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void not_(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
// comparison operators:
void isEqual(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void isNotEqual(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void isLessThan(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void isGreaterThan(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void isLessOrEqual(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void isGreaterOrEqual(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
// Set operations:
void card(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void setOp(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void contains(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void calcTransp(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);

void addA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void subA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void mulA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void divA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);
void modA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);

// just throw away all parameters:
void nop(t_sgScript* pThis, t_int countArgs, t_atom* pArgs);

#endif
