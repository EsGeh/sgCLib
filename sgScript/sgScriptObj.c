#include "sgScriptObj.h"

#include <stdlib.h>

DEF_LIST(ListAtom,ElementAtom,t_atom,copybytes,getbytes,freebytes);
DEF_LIST(ListCommand,ElementCommand,CommandInfo,copybytes,getbytes,freebytes);

// calls the function that is on the cmdStack
void callFunction(t_sgScript* pThis, FunctionInfo* pFunctionInfo, t_int countParam);

static t_class* sgScriptClass;


#define FUNCTION_COUNT 44

t_atom leftParenthesis, rightParenthesis;
FunctionInfo listFunctionInfo[FUNCTION_COUNT];

FunctionInfo* pNOP;
FunctionInfo* pRETURN_ALL;

#define FINFO_INDEX(INDEX,NAME,PARAMCOUNT,EXECAFTER,PFUNC)\
{\
	t_atom atomName;\
	SETSYMBOL( &atomName, gensym(NAME));\
	listFunctionInfo[INDEX] . name = atomName;\
	listFunctionInfo[INDEX] . paramCount = PARAMCOUNT ;\
	listFunctionInfo[INDEX] . executeAfter = EXECAFTER ;\
	listFunctionInfo[INDEX] . pFunc = PFUNC;\
}

t_class* sgScriptObjInit()
{
	FINFO_INDEX(0,"NOP",-1,-1,&nop);
	FINFO_INDEX(1,"Add",2,-1,&add);
	FINFO_INDEX(2,"Sub",2,-1,&sub);
	FINFO_INDEX(3,"Mul",2,-1,&mul);
	FINFO_INDEX(4,"Div",2,-1,&div_);
	FINFO_INDEX(5,"Mod",2,-1,&mod);
	FINFO_INDEX(6,"Print",-1,-1,&print);
	FINFO_INDEX(7,"Pack",-1,-1,&pack);
	FINFO_INDEX(8,"Out",0,-1,&out);
	FINFO_INDEX(9,"Var",-1,-1,&addVar);
	FINFO_INDEX(10,"Get",1,-1,&getVar);
	FINFO_INDEX(11,"GetA",2,-1,&getVarA);
	FINFO_INDEX(12,"Set",-1,-1,&setVar);
	FINFO_INDEX(13,"SetA",-1,-1,&setVarA);
	FINFO_INDEX(14,"If",-1,1,&if_);
	FINFO_INDEX(15,"VarMain",-1,-1,&addMainVar);
	FINFO_INDEX(16,"ClearMain",0,-1,&clearMain);
	// sgScales:
	// a | b , c , x
	FINFO_INDEX(17,"sgFunc",4,-1,&sgFunc);
	// # , a | b , c 
	FINFO_INDEX(18,"sgScale",4,-1,&sgScale);
	// boolean operators:

	FINFO_INDEX(19,"&&",2,-1,&and_);
	FINFO_INDEX(20,"||",2,-1,&or_);
	FINFO_INDEX(21,"!",1,-1,&not_);
	// comparison operators:
	FINFO_INDEX(22,"==",2,-1,&isEqual);
	FINFO_INDEX(23,"!=",2,-1,&isNotEqual);
	FINFO_INDEX(24,"<",2,-1,&isLessThan);
	FINFO_INDEX(25,">",2,-1,&isGreaterThan);
	FINFO_INDEX(26,"<=",2,-1,&isLessOrEqual);
	FINFO_INDEX(27,">=",2,-1,&isGreaterOrEqual);
	// Set operations:
	FINFO_INDEX(28,"Setify",-1,-1,&setify);
	FINFO_INDEX(29,"Card",-1,-1,&card);
	FINFO_INDEX(30,"SetOp",-1,-1,&setOp);
	FINFO_INDEX(31,"CalcTransp",-1,-1,&calcTransp);
	FINFO_INDEX(32,"Contains",-1,-1,&contains);
	FINFO_INDEX(33,"AddA",-1,-1,&addA);
	FINFO_INDEX(34,"SubA",-1,-1,&subA);
	FINFO_INDEX(35,"MulA",-1,-1,&mulA);
	FINFO_INDEX(36,"DivA",-1,-1,&divA);
	FINFO_INDEX(37,"ModA",-1,-1,&modA);
	FINFO_INDEX(38,"Rnd",2,-1,&random_);
	FINFO_INDEX(39,"MinMax",3,-1,&sgMinMax);
	FINFO_INDEX(40,"RETURN_ALL",-1,-1,&returnAll);
	FINFO_INDEX(41,"RndI",2,-1,&rndInt);
	FINFO_INDEX(42,"Inc",1,-1,&inc);
	FINFO_INDEX(43,"Dec",1,-1,&dec);
	pNOP = &listFunctionInfo[0];
	pRETURN_ALL = &listFunctionInfo[40];

	sgScriptClass = class_new(
		gensym("sgScript"), 		// name
		(t_newmethod )sgScriptObjNew, 	// newmethod
		(t_method )sgScriptObjFree, 	// freemethod
		//0,
		sizeof(t_sgScript), 		// size
		CLASS_DEFAULT, 			// object type
		A_GIMME,			// list of atoms following the selector...
		0
	);
	class_addbang(
		sgScriptClass,
		(t_method )sgScriptObj_OnExecute
	);
	class_addlist(
		sgScriptClass,
		(t_method )sgScriptObj_OnExecMeta
	);
	SETSYMBOL( &leftParenthesis, gensym("("));
	SETSYMBOL(& rightParenthesis, gensym(")"));
	/*class_addlist(
		sgScriptClass,
		(t_method )sgScriptObj_OnSetProgram
	);*/
	class_addmethod(
		sgScriptClass,
		(t_method )sgScriptObj_OnSetProgram,
		gensym("setProgram"),
		A_GIMME
	);
	class_addanything(
		sgScriptClass,
		(t_method )sgScriptObj_OnSetVar
	);
	//class_addfloat(sgScript_class, sgScriptOnInput);
	return sgScriptClass;
}

void sgScriptObjExit()
{
}

// the constructor
/*void* sgScriptNew()
{
	t_sgScript* x = (t_sgScript* )pd_new(sgScriptClass);
	//x -> controlMessage = 0;
	
	//post("opening usb device...");
	//TRY(!usbOpen(),MSG_COULD_NOT_OPEN);
	return (void* )x;
}*/

CONSTRUCTOR(sgScript,t_symbol* name, int argc, t_atom* argv)
{
	DB_PRINT("creating sgScript object...");
	//t_sgScript* x = (t_sgScript* )pd_new(sgScript_class);
	NEW_INSTANCE(sgScript,x);
	x -> skipMode = FALSE;
	x -> currentCode = NULL;
	x -> currentProgCount = 0;
	x -> code = NULL;
	x -> cmdCount = 0;
	x -> metaCode = NULL;
	x -> metaCodeCount = 0;
	
	x -> peek = 0;
	x -> outputBufferCount = 0;
	x -> pSymbolTable = SymbolTable_New();
	//ListSTEntryInit( & x->symbolTable );
	ListAtomInit( & x->stack );
	ListCommandInit( & x->cmdStack );
	//ListAtomInit( & x -> currentCode );
	//x -> currentCode = getbytes(sizeof(t_atom)*MAX_PROGRAM_LENGTH);


	x -> pOutlet = outlet_new( & x -> obj, &s_list);
	inlet_new(
		& x -> obj,
		& x -> obj . ob_pd,
		gensym("list"),
		gensym("setProgram")
	);
	if( argc > 0 )
	{
		inlet_new(
			& x -> obj,
			& x -> obj . ob_pd,
			gensym("set"),
			gensym("onSetVar")
		);
	}
	for( int i=0; i<argc; i++)
	{
		Variable variable;
		variable . count = 1;
		variable . values = getbytes( sizeof(t_atom) );
		SETFLOAT( & variable . values[0], 0 );
		STValue value;
		value . type = VALUE;
		value . variable = variable;
		STEntry entry;
		entry . symbol = argv[i];
		entry . value = value;
		SymbolTable_AddMainVar(x -> pSymbolTable, entry);

	}
	/*x -> bufferLength = -1;
	x -> outTriggers = outlet_new(& x->obj, &s_list);
	x -> outSwitches = outlet_new(& x->obj, &s_list);
	x -> outAnalog = outlet_new(& x->obj, &s_list);
	x -> outMeta = outlet_new(&x->obj, &s_list);
	x -> outMidi = outlet_new(& x->obj, &s_list);*/
	return (void* )x;
}

DESTRUCTOR(sgScript,pThis)
{
	DB_PRINT("removing sgScript object...");
	freeProgram(pThis);
	freeMetaProgram(pThis);
	SymbolTable_Free( pThis -> pSymbolTable );
	ListCommandExit( & pThis->cmdStack );
	ListAtomExit( & pThis->stack );
	//ListSTEntryExit( & pThis->symbolTable );
	//freebytes(x -> currentCode);
	//ListAtomExit( & pThis -> currentCode );
}

//DECL_LIST(ListInt,ElementInt,t_int,copybytes,getbytes,freebytes);
//DEF_LIST(ListInt,ElementInt,t_int,copybytes,getbytes,freebytes);

void sgScriptObj_OnExecute(t_sgScript* pThis)
{
	pThis -> currentProgCount = pThis-> cmdCount;
	pThis -> currentCode = pThis -> code;
	sgScriptObj_exec( pThis );
}

void sgScriptObj_OnExecMeta(t_sgScript* pThis, t_symbol* s, int argc, t_atom* argv)
{
	DB_PRINT("sgScriptObj_OnExecMeta");
	freeMetaProgram(pThis);
	/*if( pThis -> currentCode != pThis -> code )
		freebytes(pThis -> currentCode, pThis->currentProgCount * sizeof(Command));*/
	pThis -> metaCode = getbytes(sizeof(t_atom) * argc);
	memcpy( pThis->metaCode, argv, sizeof(t_atom) * argc);
	pThis -> metaCodeCount = argc;

	pThis -> currentCode = pThis -> metaCode;
	pThis -> currentProgCount = pThis -> metaCodeCount;

	sgScriptObj_exec( pThis );
}

void sgScriptObj_OnSetProgram(t_sgScript* pThis, t_symbol* s, int argc, t_atom* argv)
{
	DB_PRINT("sgScriptObj_OnSetProgram");
	freeProgram(pThis);
	pThis -> code = copybytes(argv, sizeof(t_atom)*argc);
	pThis -> cmdCount = argc;
}
void sgScriptObj_OnSetVar(t_sgScript* pThis, t_symbol* s, int argc, t_atom* argv)
{
	DB_PRINT("sgScriptObj_OnSetVar");
	t_atom* prog = getbytes( sizeof(t_atom)* (argc+3) );
	SETSYMBOL( &prog[0], gensym("Set"));
	SETSYMBOL( &prog[1], atom_getsymbol(&leftParenthesis));
	prog[2] = argv[0];
	memcpy( &prog[3], &argv[1], sizeof(t_atom)*(argc-1));
	SETSYMBOL( &prog[3+argc-1], atom_getsymbol(&rightParenthesis));

	pThis -> currentCode = prog;
	pThis -> currentProgCount = 3+argc;
	sgScriptObj_exec( pThis );
	freebytes( prog, sizeof(t_atom) * (argc+3) );
}

// executes the current code:
void sgScriptObj_exec(t_sgScript* pThis)
{
	int countParenthesisRightIgnore = 1;
	pThis -> skipMode = FALSE;
	pThis -> peek = 0;
	/*SymbolTable_Exit( pThis -> pSymbolTable );
	SymbolTable_Init( pThis -> pSymbolTable );*/
	SymbolTable_Clear( pThis -> pSymbolTable );

	ElementCommand* pElCurrentFunction = NULL; // topmost function on the command stack
	//int index
	//CommandInfo* pCurrentFunction = NULL;
	t_atom* pCurrentToken=NULL;
	while( (pCurrentToken = lexer_getNextToken(pThis)) )
	{
		char buf[256];
		atom_string(pCurrentToken, buf, 256);
		DB_PRINT("current Token: '%s'", buf);
		FunctionInfo* pFunctionInfo = NULL;
		pFunctionInfo = getFunctionInfo( pCurrentToken );
		if( pFunctionInfo )
		{
			if( ! pThis->skipMode )
			{
				// <func>
				DB_PRINT("Is a function");
				// add the function to the command stack:
				CommandInfo* pCurrentCommandInfo = getbytes(sizeof(CommandInfo));
				//pCurrentCommandInfo -> name = *pCurrentToken;
				//pCurrentCommandInfo -> countParam = -1;
				pCurrentCommandInfo -> stackHeight0 = ListAtomGetSize ( & pThis -> stack );
				pCurrentCommandInfo -> pFunctionInfo = pFunctionInfo;
				pElCurrentFunction = ListCommandAdd ( & pThis -> cmdStack, pCurrentCommandInfo);
			}
			// eat the "(" that follows:
			if( ! lexer_consumeNextToken(pThis, & leftParenthesis) )
			{
				// report syntax error:
				POST("ERROR: '(' expected, after ...");
			}
			else if( pThis->skipMode )
				countParenthesisRightIgnore ++;
		}
		else if (compareAtoms(pCurrentToken,&rightParenthesis))
		{
			if( pThis->skipMode )
			{
				countParenthesisRightIgnore --;
				if( countParenthesisRightIgnore == 0 )
				{
					pThis -> skipMode = FALSE;
					countParenthesisRightIgnore = 1;
				}
				continue;
			}
			// ")"
			if (pElCurrentFunction)
			{
				FunctionInfo* pFunctionInfo = pElCurrentFunction -> pData -> pFunctionInfo;
				t_int paramCount =
					ListAtomGetSize ( & pThis -> stack )
					- pElCurrentFunction -> pData -> stackHeight0
				;
				if(
					( pFunctionInfo -> paramCount == -1 )
					|| ( pFunctionInfo -> paramCount == paramCount )
				)
				{
					// execute the function:
					callFunction(pThis, pFunctionInfo, paramCount);
					pElCurrentFunction = ListCommandGetLast ( & pThis -> cmdStack );
				}
				else
					POST("ERROR: wrong number of parameters");

				//delete the function from Stack, becaus it has been executed:
				ListCommandDel( &pThis -> cmdStack, pElCurrentFunction);
				pElCurrentFunction = ListCommandGetLast ( & pThis -> cmdStack );
				tryToExecuteImmediately(pThis);
				pElCurrentFunction = ListCommandGetLast ( & pThis -> cmdStack );
			}
			else
				POST("ERROR: ')' found, but no corresponding 'func ('");
		}
		else
		{
			DB_PRINT("is a value");
			if( pThis->skipMode )
				continue;
			if (isValue(pCurrentToken))
			{
				// put value on stack:
				t_atom* pValue = getbytes(sizeof(t_atom));
				*pValue = *pCurrentToken;
				ListAtomAdd( & pThis -> stack, pValue);
			}
			else
			{
				POST("ERROR: token is neither function, var, or procedure: '%s'",buf);
			}
			tryToExecuteImmediately(pThis);
			pElCurrentFunction = ListCommandGetLast ( & pThis -> cmdStack );
		}
	}
}

// lexer methods:
t_atom* lexer_getNextToken(t_sgScript* pThis)
{
	t_atom* pRet = 0;
	if( pThis -> peek < pThis -> currentProgCount )
	{
		pRet = & pThis -> currentCode[ pThis -> peek ];
		pThis -> peek += 1;
	}
	return pRet;
}

TokenFits lexer_consumeNextToken(t_sgScript* pThis, t_atom* pExpectedSym)
{
	t_atom* pNextToken = lexer_getNextToken(pThis);
	return compareAtoms(pNextToken, pExpectedSym );
}

void lexer_setIP(t_sgScript* pThis, t_int peek)
{
}

void callFunction(t_sgScript* pThis, FunctionInfo* pFunctionInfo, t_int countParam)
{
	// create an array of pointers to the parameters:
	//t_atom** pArrayParam = getbytes( sizeof(t_atom* )* countParam);
	// first copy params:
	t_atom* pArgs = getbytes( sizeof(t_atom )* countParam);
	{
		ElementAtom* pElOpNext = ListAtomGetLast( &pThis -> stack);
		//pArrayParam[countParam-1] = pElOpNext -> pData;
		for ( int i=countParam-1; i>=0; i--)
		{
			pArgs[i] = *(pElOpNext-> pData);
			pElOpNext = ListAtomGetPrev( &pThis -> stack, pElOpNext);
		}
	}
	// ... delete them from stack
	for ( int i=0; i<countParam; i++)
	{
		ElementAtom* pElParam = ListAtomGetLast ( &pThis -> stack );
		ListAtomDel( &pThis -> stack, pElParam );
	}
	DB_PRINT("callFunction called");
	// call command:
	(pFunctionInfo -> pFunc) (pThis, countParam, pArgs);
	// free array of parameters:
	freebytes( pArgs, sizeof(t_atom ) * countParam );
}

void freeProgram(t_sgScript* pThis)
{
	if( pThis -> code )
		freebytes(pThis -> code, pThis->cmdCount * sizeof(Command));
}
void freeMetaProgram(t_sgScript* pThis)
{
	if( pThis -> metaCode )
		freebytes(pThis -> metaCode, pThis->metaCodeCount * sizeof(Command));
}

FunctionInfo* getFunctionInfo(t_atom* pName)
{
	for(int i=0; i<FUNCTION_COUNT; i++)
	{
		if( compareAtoms( & listFunctionInfo[i] . name, pName))
			return & listFunctionInfo[i];
	}
	return NULL;
}

BOOL isValue(t_atom* pToken)
{
	if( atomEqualsString( pToken, "(" ) )
		return FALSE;
	return TRUE;
}

void tryToExecuteImmediately(t_sgScript* pThis)
{
	ElementCommand* pElCurrentFunction = ListCommandGetLast ( & pThis -> cmdStack );
	if( !pElCurrentFunction )
		return ;
	// if the topmost command is a dont-read-all-parameters-command:
	if( pElCurrentFunction->pData -> pFunctionInfo -> executeAfter != -1)
	{
		DB_PRINT("trying to executed immediately...:");
		FunctionInfo* pFunctionInfo = pElCurrentFunction->pData -> pFunctionInfo;
		// check if there are enough values on stack now to call the next function
		t_int paramCount =
			ListAtomGetSize ( & pThis -> stack )
			- pElCurrentFunction -> pData -> stackHeight0
		;
		if( paramCount== pFunctionInfo -> executeAfter )
		{
			callFunction(pThis, pFunctionInfo, paramCount);
		}
		else if( paramCount > pFunctionInfo -> executeAfter)
		{
			char buf[256];
			atom_string(& pFunctionInfo->name, buf, 256);
			POST("ERROR: wrong number of parameters for %s", buf);
		}
		ListCommandDel( &pThis -> cmdStack, pElCurrentFunction);
	}
}

/*BOOL isVar(t_atom* pToken)
{
	return FALSE;
}

BOOL isProc(t_atom* pToken)
{
	return FALSE;
}*/

void add(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	DB_PRINT("add");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(& pArgs[0]) + atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( &pThis -> stack, pResult);
}


void sub(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	DB_PRINT("sub");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(&pArgs[0]) - atom_getfloat(&pArgs[1]));
	//push result on stack:
	ListAtomAdd( &pThis -> stack, pResult);
}
void mul(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	DB_PRINT("mul");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(&pArgs[0]) * atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( &pThis -> stack, pResult);
}
void div_(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	DB_PRINT("div");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, atom_getfloat(&pArgs[0]) / atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( &pThis -> stack, pResult);
}
void mod(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	DB_PRINT("div");
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, (t_int )atom_getfloat(&pArgs[0]) % (t_int )atom_getfloat(& pArgs[1]));
	//push result on stack:
	ListAtomAdd( &pThis -> stack, pResult);
}

void print(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	for ( int i=0; i<countArgs; i++)
	{
		pThis -> outputBuffer [ pThis -> outputBufferCount ] = (pArgs[i]);
		pThis -> outputBufferCount ++ ;
	}
}

void pack(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_atom** pResult = getbytes(sizeof(t_atom* )* (countArgs + 2));
	pResult[0] = getbytes( sizeof(t_atom) );
	*pResult[0] = pArgs[0];
	t_atom atomSizePack;
	SETFLOAT( &atomSizePack, countArgs-1 );
	pResult[1] = getbytes( sizeof(t_atom) );
	*pResult[1] = atomSizePack;
	for(int i=0; i< countArgs-1; i++)
	{
		pResult[2+i] = getbytes( sizeof(t_atom) );
		* pResult[2+i] = pArgs[i+1];
	}
	ListAtomAdd( &pThis -> stack, pResult[0]);
	ListAtomAdd( &pThis -> stack, pResult[1]);
	for(int i=0; i< countArgs-1; i++)
	{
		ListAtomAdd( & pThis -> stack, pResult[2+i]);
	}
	freebytes( pResult, sizeof(t_atom* ) * (countArgs+2));
}

void out(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	outlet_list(
		pThis -> pOutlet,
		&s_list,
		pThis -> outputBufferCount,
		pThis -> outputBuffer
		);
	pThis -> outputBufferCount = 0;
}

void addVar(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		POST("WARNING: addVar with zero parameters called!");
		return;
	}
	else if( countArgs == 1 )
	{
		POST("WARNING: addVar with one parameters called!");
		return;
	}
	Variable variable;
	variable . count = countArgs - 1;
	variable . values = getbytes( sizeof(t_atom)* (countArgs-1) );
	for( int i=0; i<(countArgs-1); i++ )
	{
		variable . values[i] = pArgs[i+1];
	}
	STValue value;
	value . type = VALUE;
	value . variable = variable;
	STEntry entry;
	entry . symbol = pArgs[0];
	entry . value = value;
	SymbolTable_Add(pThis -> pSymbolTable, entry);
	//addToSymbolTable( pThis, countArgs-1 , & pArgs[1]);
}


void getVar(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	if( ! pSTValue )
	{
		char buf[256];
		atom_string( & pArgs[0], buf, 256 );
		POST("ERROR: getVar: couldn't find variable \"%s\"", buf);
		return;
	}
	if( pSTValue -> type == VALUE)
	{
		Variable* pVar = & pSTValue -> variable;
		for( int i=0; i< pVar->count; i++ )
		{
			t_atom* pResult = getbytes(sizeof(t_atom));
			(*pResult) =  pSTValue -> variable . values[i];
			ListAtomAdd( &pThis -> stack, pResult);
		}
		//return pSTValue -> value;
	}
	else
		POST("ERROR: symbol table entry is not a value!");
	
}

void getVarA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	t_int index = atom_getfloat( & pArgs[1] );
	if( ! pSTValue )
	{
		char buf[256];
		atom_string( & pArgs[0], buf, 256 );
		POST("ERROR: getVar: couldn't find variable \"%s\"", buf);
		return;
	}
	if( pSTValue -> type == VALUE)
	{
		Variable* pVar = & pSTValue -> variable;

		if( index < 0 || index >= pVar->count )
		{
			POST("ERROR: getVarA: index out of bounds: \"%i\"", (int )index);
			return;
		}
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) =  pSTValue -> variable . values[index];
		ListAtomAdd( &pThis -> stack, pResult);
		//return pSTValue -> value;
	}
	else
		POST("ERROR: symbol table entry is not a value!");
	
}

//void setVar(t_sgScript* pThis, t_atom* pName, t_atom* pValue)
void setVar(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		POST("WARNING: setVar called with zero parameters");
		return;
	}
	DB_PRINT("setVar called with %i args", countArgs);

	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	if( !pSTValue )
	{
		char buf[256];
		atom_string( & pArgs[0],buf,256);
		POST("ERROR: variable \"%s\" not found!", buf);
		return;
	}
	Variable* pVar = & pSTValue -> variable;
	freebytes( pVar->values, sizeof(t_atom)* pVar->count);
	pVar->values = getbytes( sizeof(t_atom)* (countArgs-1));
	pVar->count = countArgs-1;
	for( int i=0; i<(countArgs-1); i++ )
	{
		pVar->values[i] = pArgs[i+1];
	}
}

void setVarA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_int index = atom_getfloat( &pArgs[1] );
	t_atom* pNewValue = & pArgs[2];

	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	if( !pSTValue )
	{
		char buf[256];
		atom_string(& pArgs[0],buf,256);
		POST("ERROR: variable \"%s\" not found!", buf);
		return;
	}
	Variable* pVar = & pSTValue -> variable;
	if( index < 0 || index >= pVar->count )
	{
		POST("ERROR: setVarA: index out of bounds: \"%i\"", (int )index);
		return;
	}
	pVar->values[index] = *pNewValue;

}

void addMainVar(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	//POST("addMainVar");
	if( countArgs == 0 )
	{
		POST("WARNING: addMainVar with zero parameters called!");
		return;
	}
	else if( countArgs == 1 )
	{
		POST("WARNING: addMainVar with one parameters called!");
		return;
	}
	Variable variable;
	variable . count = countArgs - 1;
	variable . values = getbytes( sizeof(t_atom)* (countArgs-1) );
	for( int i=0; i<(countArgs-1); i++ )
	{
		variable . values[i] = pArgs[i+1];
	}
	STValue value;
	value . type = VALUE;
	value . variable = variable;
	STEntry entry;
	entry . symbol = pArgs[0];
	entry . value = value;
	SymbolTable_AddMainVar(pThis -> pSymbolTable, entry);
}
void clearMain(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	//POST("clearMain");
	SymbolTable_Exit( pThis -> pSymbolTable );
	SymbolTable_Init( pThis -> pSymbolTable );
}

void if_(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	//POST("if_ called!!");
	if( atom_getfloat(& pArgs[0]) )
	{
		CommandInfo* pCurrentCommandInfo = getbytes(sizeof(CommandInfo));
		pCurrentCommandInfo -> stackHeight0 = ListAtomGetSize ( & pThis -> stack );
		pCurrentCommandInfo -> pFunctionInfo = pRETURN_ALL;
		ListCommandAdd ( & pThis -> cmdStack, pCurrentCommandInfo);
	}
	else
	{
		pThis->skipMode = TRUE;
	}
}

// sgScales:
void sgFunc(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float x = atom_getfloat( & pArgs[0] );
	t_float stepAdd = atom_getfloat( & pArgs[1] );
	t_float step0 = atom_getfloat( & pArgs[2] );
	t_float c = atom_getfloat( & pArgs[3] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, c + stepAdd/2 * x*x + ( step0 - stepAdd/2 ) * x );
	ListAtomAdd( &pThis -> stack, pResult);
}
void sgScale(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float n = atom_getfloat( & pArgs[0] );
	t_float stepAdd = atom_getfloat( & pArgs[1] );
	t_float step0 = atom_getfloat( & pArgs[2] );
	t_float c = atom_getfloat( & pArgs[3] );

	for( int i=0; i<n; i++ )
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, c + stepAdd/2 * i*i + ( step0 - stepAdd/2 ) * i );
		ListAtomAdd( &pThis -> stack, pResult);
	}
}

// boolean operators:
void and_(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a && b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void or_(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a || b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void not_(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, ! a );
	ListAtomAdd( &pThis -> stack, pResult);
}

// comparison operators:
void isEqual(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a == b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void isNotEqual(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a != b );
	ListAtomAdd( &pThis -> stack, pResult);

}
void isLessThan(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a < b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void isGreaterThan(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a > b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void isLessOrEqual(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a <= b );
	ListAtomAdd( &pThis -> stack, pResult);
}
void isGreaterOrEqual(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float a = atom_getfloat( & pArgs[0] );
	t_float b = atom_getfloat( & pArgs[1] );

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a >= b );
	ListAtomAdd( &pThis -> stack, pResult);
}

typedef enum ESetOp { UNION, MINUS } SetOp;
// Set operations:
BOOL AtomListCompareAtoms(t_atom* pInList, t_atom* p)
{
	return compareAtoms(pInList, p);
}
BOOL setContains(t_int count, t_atom* set, t_atom* element);
BOOL listContains(ListAtom* pList, t_atom* pElement);

void setify(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	for( int i=0; i<countArgs; i++ )
	{
		t_atom* pCurrent = & pArgs[i];
		if( ! setContains( i-1, pArgs, pCurrent ) )
		{
			t_atom* pResult = getbytes(sizeof(t_atom));
			*pResult = *pCurrent;
			ListAtomAdd( &pThis -> stack, pResult);
		}
	}
}
void card(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, countArgs );
	ListAtomAdd( &pThis -> stack, pResult);
}

void setOp(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	ListAtom listReturn;
	ListAtomInit( & listReturn );
	// brute force.
	// a better solution would be to sort both lists, and then to merge them:
	t_int countFirst = 0;
	BOOL afterOp = FALSE;
	SetOp op;
	for( int i=0; i<countArgs; i++ )
	{
		t_atom* pCurrent = & pArgs[i];
		if( afterOp == FALSE )
		{
			//if( atom_getsymbol( pCurrent ) == gensym("union") )
			if( atomEqualsString( pCurrent, "union" ) )
			{
				op = UNION;
				afterOp = TRUE;
			}
			else if( atomEqualsString( pCurrent, "minus" ) )
			{
				op = MINUS;
				afterOp = TRUE;
			}
			else
			{
				countFirst ++ ;
				t_atom* pAdd = getbytes(sizeof(t_atom));
				(* pAdd) = * pCurrent;
				ListAtomAdd( & listReturn, pAdd );
			}
		}
		else
		{
			ElementAtom* pElToDelete = NULL;
			switch( op )
			{
				case UNION:
					if( ! listContains( &listReturn, pCurrent ) )
					{
						t_atom* pAdd = getbytes(sizeof(t_atom));
						(* pAdd) = * pCurrent;
						ListAtomAdd( & listReturn, pAdd );
					}
				break;
				case MINUS:
					pElToDelete = ListAtomGetElement( &listReturn, pCurrent, AtomListCompareAtoms);
					if( pElToDelete )
					{
						/*t_atom* pAdd = getbytes(sizeof(t_atom));
						(* pAdd) = * pCurrent;
						ListAtomAdd( & listReturn, pAdd );*/
						ListAtomDel( & listReturn, pElToDelete );
					}
				break;
			}
		}
	}
	ElementAtom* pCurrent = ListAtomGetFirst( & listReturn );
	while ( pCurrent )
	{
		/*char buf[256];
		atom_string( pCurrent->pData, buf, 256);
		POST("current: %s", buf);*/
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) = (* pCurrent -> pData);
		ListAtomAdd( &pThis -> stack, pResult);
		
		pCurrent = ListAtomGetNext( & listReturn, pCurrent );
	}
	ListAtomExit( & listReturn );
}

BOOL listContains(ListAtom* pList, t_atom* pElement)
{
	ElementAtom* pElCurrent = ListAtomGetFirst( pList );
	while( pElCurrent )
	{
		if( atom_getfloat( pElCurrent->pData) == atom_getfloat( pElement ) )
			return TRUE;
		pElCurrent = ListAtomGetNext( pList, pElCurrent );
	}
	return FALSE;
}

void contains(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_atom element = ( pArgs[0] );
	BOOL bRet = setContains( countArgs-1, & pArgs[1], &element);

	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, bRet );
	ListAtomAdd( &pThis -> stack, pResult);
}

BOOL setContains(t_int count, t_atom* set, t_atom* element)
{
	for( int i=0; i<count; i++ )
	{
		if( atom_getfloat(& set[i]) == atom_getfloat( element ))
			return TRUE;
	}
	return FALSE;
}

void calcTransp(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	// the sets should be separated by |
	ListAtom listReturn;
	ListAtomInit( & listReturn );
	// brute force.
	// a better solution would be to sort both lists, and then to merge them:
	t_int countFirst = 0;
	for( int i=0; i<countArgs; i++ )
	{
		t_atom* pCurrent = & pArgs[i];
		if( atomEqualsString( pCurrent, "|" ) )
		{
			break;
		}
		else
		{
			countFirst ++ ;
		}
	}
	if( countFirst == countArgs )
	{
		POST("ERROR: separator \"|\" not found!");
		return;
	}
	for( int transp=0; transp<12; transp++ )
	{
		BOOL passed = TRUE;
		for( int i=countFirst+1; i<countArgs; i++ )
		{
			t_atom current;
			SETFLOAT( & current, (t_int )(atom_getfloat( & pArgs[i]) + transp) % 12 );
			if( setContains( countArgs-countFirst-1, pArgs, & current ) )
			{
				passed = FALSE;
			}
		}
		if( passed )
		{
			t_atom* pResult = getbytes(sizeof(t_atom));
			SETFLOAT( pResult, transp );
			ListAtomAdd( & listReturn, pResult );
		}
	}
	ElementAtom* pCurrent = ListAtomGetFirst( & listReturn );
	while ( pCurrent )
	{
		/*char buf[256];
		atom_string( pCurrent->pData, buf, 256);
		POST("current: %s", buf);*/
		t_atom* pResult = getbytes(sizeof(t_atom));
		(*pResult) = (* pCurrent -> pData);
		ListAtomAdd( &pThis -> stack, pResult);
		
		pCurrent = ListAtomGetNext( & listReturn, pCurrent );
	}
	ListAtomExit( & listReturn );
}

void addA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		POST("ERROR: addA called with no parameters!");
		return;
	}
	t_atom a = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, atom_getfloat(&x) + atom_getfloat(&a) );
		ListAtomAdd( &pThis -> stack, pResult);
	}
}
void subA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		POST("ERROR: subA called with no parameters!");
		return;
	}
	t_atom a = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, atom_getfloat(&x) - atom_getfloat(&a) );
		ListAtomAdd( &pThis -> stack, pResult);
	}
}
void mulA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		POST("ERROR: mulA called with no parameters!");
		return;
	}
	t_atom a = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, atom_getfloat(&x) * atom_getfloat(&a) );
		ListAtomAdd( &pThis -> stack, pResult);
	}
}
void divA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		POST("ERROR: divA called with no parameters!");
		return;
	}
	t_atom a = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, atom_getfloat(&x) / atom_getfloat(&a) );
		ListAtomAdd( &pThis -> stack, pResult);
	}
}

void modA(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		POST("ERROR: modA called with no parameters!");
		return;
	}
	/*t_float m = atom_getfloat( & pArgs[0] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, a  b );
	ListAtomAdd( &pThis -> stack, pResult)*/
	t_atom m = pArgs[0] ;
	for( int i=1; i<countArgs; i++)
	{
		t_atom x = pArgs[i] ;
		t_atom* pResult = getbytes(sizeof(t_atom));
		SETFLOAT( pResult, (t_int )atom_getfloat(&x) % (t_int )atom_getfloat(&m) );
		ListAtomAdd( &pThis -> stack, pResult);
	}
}

void sgMinMax(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float pMin = atom_getfloat( & pArgs[0] );
	t_float pMax = atom_getfloat( & pArgs[1] );
	t_float pInput = atom_getfloat( & pArgs[2] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT(
		pResult,
		((pMax) - (pMin)) * (pInput) + (pMin)
	);
	ListAtomAdd( &pThis -> stack, pResult);
}

// random
// Rand ( )
void random_(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_float min = atom_getfloat( & pArgs[0] );
	t_float max = atom_getfloat( & pArgs[1] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, min + (rand() % 1000000)/1000000.0 * (max-min) );
	ListAtomAdd( &pThis -> stack, pResult);
}

void nop(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
}

void returnAll(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	for( int i=0; i<countArgs; i++ )
	{
		t_atom* pResult = getbytes(sizeof(t_atom));
		*pResult = pArgs[i] ;
		ListAtomAdd( &pThis -> stack, pResult);
	}
}


void rndInt(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	t_int min = atom_getfloat( & pArgs[0] );
	t_int max = atom_getfloat( & pArgs[1] );
	t_atom* pResult = getbytes(sizeof(t_atom));
	SETFLOAT( pResult, min + (rand() % (max - min + 1) ) );
	ListAtomAdd( &pThis -> stack, pResult);
}
void inc(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		POST("WARNING: inc called with zero parameters");
		return;
	}
	DB_PRINT("inc called with %i args", countArgs);

	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	if( !pSTValue )
	{
		char buf[256];
		atom_string( & pArgs[0],buf,256);
		POST("ERROR: inc: variable \"%s\" not found!", buf);
		return;
	}
	Variable* pVar = & pSTValue -> variable;
	t_float oldVal = atom_getfloat( & pVar -> values [0] );
	freebytes( pVar->values, sizeof(t_atom)* pVar->count);
	pVar->values = getbytes( sizeof(t_atom) );
	pVar->count = 1;
	SETFLOAT( & pVar->values [0], oldVal+1 );
	/*for( int i=0; i<(countArgs-1); i++ )
	{
		pVar->values[i] = pArgs[i+1];
	}*/
}
void dec(t_sgScript* pThis, t_int countArgs, t_atom* pArgs)
{
	if( countArgs == 0 )
	{
		POST("WARNING: dec called with zero parameters");
		return;
	}
	DB_PRINT("dec called with %i args", countArgs);

	STValue* pSTValue = SymbolTable_Lookup( pThis -> pSymbolTable, & pArgs[0] );
	if( !pSTValue )
	{
		char buf[256];
		atom_string( & pArgs[0],buf,256);
		POST("ERROR: dec: variable \"%s\" not found!", buf);
		return;
	}
	Variable* pVar = & pSTValue -> variable;
	t_float oldVal = atom_getfloat( & pVar -> values [0] );
	freebytes( pVar->values, sizeof(t_atom)* pVar->count);
	pVar->values = getbytes( sizeof(t_atom) );
	pVar->count = 1;
	SETFLOAT( & pVar->values [0], oldVal-1 );
	/*for( int i=0; i<(countArgs-1); i++ )
	{
		pVar->values[i] = pArgs[i+1];
	}*/
}
