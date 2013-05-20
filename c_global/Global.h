#ifndef _GLOBAL_H_
#define _GLOBAL_H_


#define BOOL int
#define TRUE 1
#define FALSE 0

#include "m_pd.h"
#include "LinkedList.h"
#include <string.h>
#define CLASSNAME(name) name##Class
#define TYPENAME(name) t_##name

#define CONSTRUCTOR(name, ...) void* name##ObjNew(__VA_ARGS__)
#define DESTRUCTOR(name,pThis) void name##ObjFree(TYPENAME(name)* pThis)
#define NEW_INSTANCE(name,varname) TYPENAME(name)* varname = (TYPENAME(name)* )pd_new(CLASSNAME(name))


#include <stdio.h>


#define IN_RANGE(x, min,sup) (x>=min && x<=sup)

#define MAX_MESSAGE_LENGTH 256
#define POST(msg, ...)\
	do\
	{\
		char __buf[MAX_MESSAGE_LENGTH];\
		sprintf(__buf, msg, ## __VA_ARGS__);\
		post(__buf);\
	} while(0);

#define TRY(code,errorMsg)\
	do \
	{\
		if(!(code))\
		{\
			error(errorMsg);\
		}\
	}\
	while(0);

#define TRY_RET(code,errorMsg,ret)\
	do \
	{\
		if(!(code))\
		{\
			error(errorMsg);\
			return ret;\
		}\
	}\
	while(0);

INLINE int compareAtoms(t_atom* atoml, t_atom* atomr)
{
	if(atoml -> a_type == A_SYMBOL)
	{
		if(atomr -> a_type == A_SYMBOL)
		{
			char bufl[256], bufr[256];
			atom_string(atoml,bufl,256);
			atom_string(atomr,bufr,256);
			return !strncmp(bufl,bufr,256);
		}
	}
	else
	{
		if(atomr -> a_type == A_FLOAT)
		{
			return atom_getfloat(atoml)==atom_getfloat(atomr);
		}
	}
	return 0;
}

INLINE BOOL atomEqualsString(t_atom* pAtom, char* string)
{
	char buf[256];
	atom_string(pAtom,buf,256);
	if(!strncmp(buf,string,256))
		return TRUE;
	return FALSE;
}

#endif
