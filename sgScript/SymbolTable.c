#include "SymbolTable.h"

#include <math.h>

#define BASESIZE 2

struct SSymbolTable
{
	t_int entryCount;
	STEntry* entries;
	t_int mainVarsCount;
	STEntry* mainVars;
};


t_int getAllocatedSize( SymbolTable* pThis );
t_int getAllocatedSizeMainVars( SymbolTable* pThis );

SymbolTable* SymbolTable_New()
{
	SymbolTable* pRet = getbytes(sizeof(SymbolTable));
	SymbolTable_Init( pRet );
	return pRet;
}
void SymbolTable_Free(SymbolTable* pSymbolTable)
{
	SymbolTable_Exit( pSymbolTable );
	freebytes(pSymbolTable, sizeof(SymbolTable));
}

void SymbolTable_Init (SymbolTable* pThis)
{
	pThis -> entries = getbytes( sizeof(STEntry)* BASESIZE );
	pThis -> mainVars = getbytes( sizeof(STEntry)* BASESIZE );
	pThis -> entryCount = 0;
	pThis -> mainVarsCount = 0;
}
void SymbolTable_Exit (SymbolTable* pThis)
{
	for( int i=0; i< pThis->entryCount; i++)
	{
		STValue* pCurrent = & pThis->entries [i] . value;
		if( pCurrent->type == VALUE )
		{
			Variable* pVar = & pCurrent -> variable;
			freebytes( pVar-> values, sizeof(t_atom)*pVar->count);
		}
	}
	for( int i=0; i< pThis->mainVarsCount; i++)
	{
		STValue* pCurrent = & pThis->mainVars [i] . value;
		if( pCurrent->type == VALUE )
		{
			Variable* pVar = & pCurrent -> variable;
			freebytes( pVar-> values, sizeof(t_atom)*pVar->count);
		}
	}
	freebytes( pThis -> entries, sizeof(STEntry)* getAllocatedSize(pThis));
	freebytes( pThis -> mainVars, sizeof(STEntry)* getAllocatedSizeMainVars(pThis));
	pThis -> entryCount = 0;
	pThis -> mainVarsCount = 0;
}
/*void SymbolTable_AddVar (SymbolTable* pThis, t_atom symbol, t_int count, t_atom* values)
{
}
void SymbolTable_SetVar (SymbolTable* pThis, t_atom symbol, t_int count, t_atom* values)
{
}*/
void SymbolTable_Add (SymbolTable* pThis, STEntry Entry)
{
	t_int iAllocated = getAllocatedSize(pThis);
	if ( pThis -> entryCount >= iAllocated)
	{
		POST("double size of the symbol table...");
		doubleSize( pThis );
	}
	pThis -> entries [ pThis -> entryCount ] = Entry;
	pThis -> entryCount ++;
}
void SymbolTable_Del (SymbolTable* pThis, STEntry Entry)
{
	POST("SymbolTable_Del not yet implemented!");
}
void SymbolTable_AddMainVar(SymbolTable* pThis, STEntry Entry)
{
	t_int iAllocated = getAllocatedSizeMainVars(pThis);
	if ( pThis -> mainVarsCount >= iAllocated)
	{
		POST("double size of the symbol table...");
		doubleSizeMainVars( pThis );
	}
	pThis -> mainVars [ pThis -> mainVarsCount ] = Entry;
	pThis -> mainVarsCount ++;
}

void SymbolTable_Clear(SymbolTable* pThis)
{
	for( int i=0; i< pThis->entryCount; i++)
	{
		STValue* pCurrent = & pThis->entries [i] . value;
		if( pCurrent->type == VALUE )
		{
			Variable* pVar = & pCurrent -> variable;
			freebytes( pVar-> values, sizeof(t_atom)*pVar->count);
		}
	}
	freebytes( pThis -> entries, sizeof(STEntry)* getAllocatedSize(pThis));
	pThis -> entries = getbytes( sizeof(STEntry)* BASESIZE );
	pThis -> entryCount = 0;
}

STValue* SymbolTable_Lookup(SymbolTable* pThis, t_atom* pSymbol)
{
	char buf[256];
	atom_string(pSymbol, buf, 256);
	POST("SymbolTable_Lookup(..,%s)",buf);
	for( int i=0; i< pThis -> mainVarsCount; i++)
	{
		STEntry* pEntryCurrent = & pThis -> mainVars [i];
		atom_string( &pEntryCurrent -> symbol, buf, 256);
		POST("entry: %s", buf);
		if( compareAtoms( & pEntryCurrent -> symbol , pSymbol))
			return & pEntryCurrent -> value;
	}
	for( int i=0; i< pThis -> entryCount; i++)
	{
		STEntry* pEntryCurrent = & pThis -> entries [i];
		atom_string( &pEntryCurrent -> symbol, buf, 256);
		POST("entry: %s", buf);
		if( compareAtoms( & pEntryCurrent -> symbol , pSymbol))
			return & pEntryCurrent -> value;
	}
	return NULL;
}

void doubleSize(SymbolTable* pThis)
{
	t_int iAllocated = getAllocatedSize(pThis);
	// realloc:
	STEntry* pNewArray = getbytes ( sizeof(STEntry)*iAllocated*2);
	//copybytes( pThis -> entries, pThis -> entryCount );
	memcpy( pNewArray, pThis -> entries, sizeof(STEntry)* pThis -> entryCount);
	freebytes( pThis -> entries, getAllocatedSize( pThis ) );
	pThis -> entries = pNewArray;
}
void doubleSizeMainVars(SymbolTable* pThis)
{
	t_int iAllocated = getAllocatedSizeMainVars(pThis);
	// realloc:
	STEntry* pNewArray = getbytes ( sizeof(STEntry)*iAllocated*2);
	//copybytes( pThis -> mainVars, pThis -> mainVarsCount );
	memcpy( pNewArray, pThis -> mainVars, sizeof(STEntry)* pThis -> mainVarsCount);
	freebytes( pThis -> mainVars, getAllocatedSizeMainVars( pThis ) );
	pThis -> mainVars = pNewArray;
}


t_int getAllocatedSize( SymbolTable* pThis )
{
	if ( pThis -> entryCount <= BASESIZE ) return BASESIZE;
	return BASESIZE * (ceil ( log2 ( pThis -> entryCount )));
}
t_int getAllocatedSizeMainVars( SymbolTable* pThis )
{
	if ( pThis -> mainVarsCount <= BASESIZE ) return BASESIZE;
	return BASESIZE * (ceil ( log2 ( pThis -> mainVarsCount )));
}
