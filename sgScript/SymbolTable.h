#ifndef _SYMBOLTABLE_H_
#define _SYMBOLTABLE_H_ 

#include "Global.h"

struct SSymbolTable;
#define SymbolTable struct SSymbolTable
#define Variable struct SVariable

typedef enum ESTValueType { VALUE } STValueType;
typedef struct SSTValue {
	STValueType type;
	union
	{
		struct SVariable {
			t_int count;
			t_atom* values;
		} variable;
	};
} STValue;
typedef struct SSTEntry {
	t_atom symbol;
	STValue value;
} STEntry;

SymbolTable* SymbolTable_New();
void SymbolTable_Free(SymbolTable* pSymbolTable);
void SymbolTable_Init (SymbolTable* pThis);
void SymbolTable_Exit (SymbolTable* pThis);

/*void SymbolTable_AddVar (SymbolTable* pThis, t_atom symbol, t_int count, t_atom* values);
void SymbolTable_SetVar (SymbolTable* pThis, t_atom symbol, t_int count, t_atom* values);*/
void SymbolTable_Add (SymbolTable* pThis, STEntry Entry);
void SymbolTable_Del (SymbolTable* pThis, STEntry Entry);
void SymbolTable_AddMainVar(SymbolTable* pThis, STEntry Entry);

// clear all variables except main variables:
void SymbolTable_Clear(SymbolTable* pThis);

STValue* SymbolTable_Lookup(SymbolTable* pThis, t_atom* pSymbol);

void doubleSize(SymbolTable* pThis);
void doubleSizeMainVars(SymbolTable* pThis);



#endif 
