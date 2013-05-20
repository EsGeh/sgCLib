#ifndef _SGPACK_H_
#define _SGPACK_H_

#include "m_pd.h"
#include "Global.h"
#include <string.h>


struct SPackOrAtom;

//struct SPackOrAtom { int test; };
//#define FREE_DATA(pack,size) freebytes(pack,size)
//#define DATA struct SPackOrAtom
//#include "LinkedList.h"


#define DECL_LIST_STD(LIST,ELEMENT,DATA)\
	DECL_LIST(LIST,ELEMENT,DATA,memcpy,getbytes,freebytes)
#define DEF_LIST_STD(LIST,ELEMENT,DATA)\
	DEF_LIST(LIST,ELEMENT,DATA,memcpy,getbytes,freebytes)


DECL_LIST_STD(ListPackOrAtom,ElementPackOrAtom,struct SPackOrAtom)

//struct List;

typedef struct SPack
{
	t_atom name;
	ListPackOrAtom listSub;
} Pack;

typedef struct SPackOrAtom
{
	t_int isAtom;
	union
	{
		t_atom atom;
		Pack pack;
	};
} PackOrAtom;

DEF_LIST_STD(ListPackOrAtom,ElementPackOrAtom,struct SPackOrAtom)

DECL_LIST_STD(ListPack,ElementPack,Pack);
DEF_LIST_STD(ListPack,ElementPack,Pack);

typedef ListPack sgData;

#define PACK(varName, atomName)\
	Pack varName;\
	varName . name = atomName ;\
	PACK_INIT(varName);
#define PACK_FROMSYM(varName, symbolName)\
	Pack varName;\
	SETSYMBOL(& varName . name, symbolName) ;\
	PACK_INIT(varName);

#define PACK_NEW(varName,atomName)\
	Pack* varName = NULL;\
	varName = (Pack* )getbytes(sizeof(Pack));\
	varName -> name = atomName;\
	PACK_INIT(*varName);
#define PACK_FROMSYM_NEW(varName,symbolName)\
	Pack* varName = NULL;\
	varName = (Pack* )getbytes(sizeof(Pack));\
	SETSYMBOL( &varName -> name, symbolName);\
	PACK_INIT(*varName);

#define PACK_INIT(pack)\
	ListPackOrAtomInit( &(pack).listSub)
#define PACK_EXIT(pack)\
	ListPackOrAtomExit( &(pack).listSub)

#define PACK_ADD_PACK(PPACK,SUBPACK)\
{\
	PackOrAtom* pPOA = getbytes(sizeof(PackOrAtom));\
	pPOA -> isAtom = 0;\
	pPOA -> pack = SUBPACK;\
	ListPackOrAtomAdd(& (PPACK)-> listSub,pPOA);\
}
#define PACK_ADD_ATOM(PPACK,ATOM)\
{\
	PackOrAtom* pPOA = getbytes(sizeof(PackOrAtom));\
	pPOA -> isAtom = 1;\
	pPOA -> atom = ATOM;\
	ListPackOrAtomAdd(& (PPACK)-> listSub,pPOA);\
}


#define PACK_SUBATOM_FROMNAME(varSub,symbolName)\
	PackOrAtom varSub;\
	varSub.isAtom = 1;\
	SETSYMBOL( & varSub.atom, symbolName )

#define PACK_SUBATOM(varSub,ATOM)\
	PackOrAtom varSub;\
	varSub.isAtom = 1;\
	varSub.atom = ATOM
	
#define PACK_SUBATOM_FROM_NAME_NEW(varSub,symbolName)\
	PackOrAtom* varSub;\
	varSub = (PackOrAtom* )getbytes(sizeof(PackOrAtom));\
	varSub -> isAtom = 1;\
	SETSYMBOL( & varSub -> atom, symbolName)

#define PACK_SUBATOM_NEW(varSub,ATOM)\
	PackOrAtom* varSub;\
	varSub = (PackOrAtom* )getbytes(sizeof(PackOrAtom));\
	varSub -> isAtom = 1;\
	varSub -> atom = ATOM

#define PACK_FROMATOMS_NEW(pack,atoms)\
	do\
	{\
		PACKHEADER_FROMATOMS(symbolName,count,pack,atoms);\
		PACK_NEW(pack,symbolName);\
	}\
	while(0)

#define UNDATA(indexCurrentAtom,atoms,atomCount,atomList,currentAtom)\
	t_int currentAtom;\
	while(currentAtom+2 <= atomCount)

#define PACKHEADER_FROMATOMS(atomName,count,atoms)\
	if( (atoms)[0].a_type != A_SYMBOL )\
		POST("Error: invalid sgPack header, first atom should be a symbol!");\
	if ((atoms)[1].a_type != A_FLOAT )\
		POST("Error: invalid sgPack header, second atom should be a float!");\
	t_atom* atomName =  & (atoms)[0];\
	t_int count = atom_getint( & (atoms)[1])



//BOOL comparePacks(

#define CHECK_HEADER(atom,expectedName)\
	{\
		char buf[256];\
		atom_string(atom,buf,256);\
		if(strncmp(buf,expectedName,256))\
			POST("ERROR: sgPack of type \"%s\" expected, but got \"%s\"!",expectedName,buf);\
	}

typedef unsigned int ListCount;

t_class* testInit();
void testExit();

void atomsFromPack(ListCount count, t_atom* listReturn, Pack* pack);
//void packFromAtoms(Pack* packReturn, ListCount count, t_atom* list);
ListCount packGetListCount(Pack* pack);

void packFree(Pack* pPack);

void atomsFromPackPrivate(ListCount count, t_atom* listReturn, Pack* pack, unsigned int* index);


typedef struct _test
{
	t_object obj;
	t_outlet* out;
} t_test;

// constructor:
CONSTRUCTOR(test,);
//DESTRUCTOR(test,pThis);
void testOnInput(t_test* pThis);

#endif
