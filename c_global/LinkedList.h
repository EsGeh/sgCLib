#ifndef _LINKED_LIST_HEADER_
#define _LINKED_LIST_HEADER_

#define INLINE static inline

#ifndef NULL
    #define NULL 0
#endif
//#define DATA int

/*
#ifndef DATA
    #warning "DATA not defined!"
#endif
*/
/*
#ifndef DATA_COPY
    #include <string.h>
    #define DATA_COPY(pDest,pSrc) \
        memcpy(pDest, pSrc, sizeof(DATA))
#endif

#ifndef MALLOC
    #include <stdlib.h>
    #define MALLOC(size) malloc(size)
#endif

#ifndef FREE
    #include <stdlib.h>
    #define FREE(p) free(p)
#endif
*/

/*#ifndef FREE_DATA
	#warning "FREE_DATA undefined!"
	#define FREE_DATA(p)
#endif*/

//DATA_COPY(src,des
//MALLOC(size)
//FREE(p,size)
#define DECL_LIST(LIST,ELEMENT,DATA,DATA_COPY,MALLOC,FREE)\
struct S##ELEMENT\
{\
    struct S##ELEMENT* pPrev;\
    struct S##ELEMENT* pNext;\
    DATA* pData;\
};\
\
typedef struct S##ELEMENT ELEMENT;\
\
struct S##LIST\
{\
	unsigned int size;\
	ELEMENT* pHead;\
};\
\
typedef struct S##LIST LIST;

#define DEF_LIST(LIST,ELEMENT,DATA,DATA_COPY,MALLOC,FREE)\
INLINE void LIST##Init(LIST* pList);\
INLINE void LIST##Exit(LIST* pList);\
\
INLINE int LIST##IsEmpty(LIST* pList);\
INLINE int LIST##GetSize(LIST* pList);\
\
INLINE ELEMENT* LIST##GetFirst(LIST* pList) ;\
INLINE ELEMENT* LIST##GetLast(LIST* pList) ;\
INLINE int LIST##HasNext(LIST* pList, ELEMENT* element);\
INLINE int LIST##HasPrev(LIST* pList, ELEMENT* element);\
INLINE ELEMENT* LIST##GetElement(LIST* pList, const DATA* pData);\
INLINE ELEMENT* LIST##GetNext(LIST* pList, ELEMENT* element) ;\
INLINE ELEMENT* LIST##GetPrev(LIST* pList, ELEMENT* element);\
\
INLINE ELEMENT* LIST##Add(LIST* pList, DATA* pData);\
INLINE ELEMENT* LIST##Insert(LIST* pList, DATA* pData, ELEMENT* pElement);\
INLINE int LIST##Del(LIST* pList, ELEMENT* element);\
\
INLINE void LIST##Init(LIST* pList)\
{\
    pList->size= 0;\
    pList->pHead= NULL;\
}\
INLINE void LIST##Exit(LIST* pList)\
{\
	if(! LIST##IsEmpty(pList))\
	{\
		ELEMENT* pCurrent= pList->pHead->pPrev;\
		pList->pHead->pPrev = NULL;\
		while(pCurrent->pPrev)\
		{\
			pCurrent = pCurrent -> pPrev;\
			FREE(pCurrent->pNext,sizeof(ELEMENT));\
			pCurrent->pNext = NULL;\
		}\
		FREE(pCurrent,sizeof(ELEMENT));\
	}\
}\
\
INLINE int LIST##IsEmpty(LIST* pList)\
{\
    return pList->pHead == NULL;\
}\
INLINE int LIST##GetSize(LIST* pList)\
{\
    return pList->size;\
}\
\
INLINE ELEMENT* LIST##GetFirst(LIST* pList)\
{\
    if(!LIST##IsEmpty(pList))\
        return pList->pHead;\
    return NULL;\
}\
INLINE ELEMENT* LIST##GetLast(LIST* pList)\
{\
    if(!LIST##IsEmpty(pList))\
        return pList->pHead->pPrev;\
    return NULL;\
}\
INLINE int LIST##HasNext(LIST* pList, ELEMENT* element)\
{\
    return (element->pNext != pList-> pHead);\
}\
INLINE int LIST##HasPrev(LIST* pList, ELEMENT* element)\
{\
    return (element != pList-> pHead);\
}\
\
INLINE ELEMENT* LIST##GetElement(LIST* pList,const DATA* pData)\
{\
	if(!LIST##IsEmpty(pList))\
	{\
		ELEMENT* pCurrent= pList-> pHead;\
		do\
		{\
			if( (pCurrent-> pData) == pData)\
				return pCurrent;\
			pCurrent = pCurrent-> pNext;\
		}\
		while(pCurrent!= pList-> pHead);\
	}\
	return NULL;\
}\
INLINE ELEMENT* LIST##GetNext(LIST* pList, ELEMENT* element)\
{\
    if(LIST##HasNext(pList,element))\
        return element->pNext;\
    return NULL;\
}\
INLINE ELEMENT* LIST##GetPrev(LIST* pList, ELEMENT* element)\
{\
    if(LIST##HasPrev(pList,element))\
        return element->pPrev;\
    return NULL;\
}\
\
INLINE ELEMENT* LIST##Add(LIST* pList, DATA* pData)\
{\
    ELEMENT* e= (ELEMENT* )MALLOC(sizeof(ELEMENT));\
    e -> pData = pData;\
	if( LIST##IsEmpty(pList))\
	{\
		pList->pHead = e;\
		e-> pPrev = e -> pNext = e;\
	}\
	else\
	{\
		ELEMENT* prev= pList->pHead->pPrev;\
		prev->pNext = e;\
		e-> pPrev= prev;\
		pList->pHead->pPrev = e;\
		e-> pNext = pList-> pHead;\
	}\
	pList->size++;\
	return e;\
}\
\
INLINE ELEMENT* LIST##Insert(LIST* pList, DATA* pData, ELEMENT* pElement)\
{\
    ELEMENT* e= (ELEMENT* )MALLOC(sizeof(ELEMENT));\
    e-> pData = pData;\
        ELEMENT* pPrev= pElement;\
        ELEMENT* pNext= pElement->pNext;\
		pPrev->pNext = e;\
		e-> pPrev= pPrev;\
		pNext->pPrev = e;\
		e-> pNext = pNext;\
		pList->size++;\
	return e;\
}\
\
INLINE int LIST##Del(LIST* pList, ELEMENT* element)\
{\
	if( pList-> pHead-> pNext != pList-> pHead ) {\
		if( element == pList-> pHead )\
			pList-> pHead = pList-> pHead-> pNext;\
		element-> pPrev->pNext = element-> pNext;\
		element-> pNext-> pPrev = element-> pPrev;\
		FREE(element -> pData,sizeof(DATA));\
		FREE(element,sizeof(ELEMENT));\
	}\
	else\
	{\
		FREE(element -> pData,sizeof(DATA));\
		FREE(element,sizeof(ELEMENT));\
		pList->pHead = NULL;\
	}\
	pList->size--;\
	return 0;\
}

#endif
