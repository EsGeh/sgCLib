#include "Pack.h"


static t_class* testClass;

t_class* testInit()
{
	testClass = class_new(
		gensym("test"),
		(t_newmethod )testObjNew,
		0,
		//testFree,
		sizeof(t_test),
		CLASS_DEFAULT,
		0
	);
	class_addbang(testClass, testOnInput);
	return testClass;
}

void testExit()
{
}

void atomsFromPack(ListCount count, t_atom* listReturn, Pack* pack)
{
	unsigned int indexCurrent=0;
	atomsFromPackPrivate(count,listReturn,pack,&indexCurrent);
}

void packFree(Pack* pPack)
{
	//POST("packFree");
	ElementPackOrAtom* pElement = ListPackOrAtomGetFirst( & pPack -> listSub );
	while( pElement)
	{
		//POST("rec");
		PackOrAtom* pPackOrAtom = pElement -> pData;
		if( ! pPackOrAtom -> isAtom )
		{
			packFree( & pPackOrAtom -> pack);
		}
		pElement = ListPackOrAtomGetNext( & pPack -> listSub, pElement);
	}
	ListPackOrAtomExit( & pPack -> listSub);
}

void atomsFromPackPrivate(ListCount count, t_atom* listReturn, Pack* pack, unsigned int* index)
{
	POST("count: %i, index: %i", count, (*index));
	listReturn[(*index)] = pack->name;
				/*char buf[256];
				atom_string(& listReturn[*index],buf, 256);
				POST("Writing: [%i] = %s",*index,buf);*/

	unsigned int indexStart = (*index);

	(*index) += 2;

	ElementPackOrAtom* pElement=ListPackOrAtomGetFirst( & pack -> listSub);
	if(pElement)
	{
		do
		{
			PackOrAtom* pPackOrAtom= pElement -> pData;
			if(pPackOrAtom->isAtom)
			{
				listReturn[*index] = pPackOrAtom->atom;
				/*char buf[256];
				atom_string(& listReturn[*index],buf, 256);
				POST("Writing: [%i] = %s",*index,buf);*/
				(*index) += 1;
			}
			else
			{
				atomsFromPackPrivate( count- (*index), listReturn, &pPackOrAtom->pack, index);
				//POST("end recursion!");
				/*POST("INFO:");
				char hurz[256];
				atom_string(listReturn+temp,hurz,256);
				post(hurz);*/
			}
			pElement = ListPackOrAtomGetNext( &pack->listSub, pElement);
		}
		while(pElement);
	}
	SETFLOAT(&listReturn[indexStart+1], (*index)-indexStart-2);
	/*char buf2[256];
	atom_string(& listReturn[indexStart+1], buf2, 256);
	POST("Writing: [%i] = %s",indexStart+1,buf2);*/

	/*
	char buf3[256] = "";
	//POST("indexStart: %i",indexStart,count
	for(int i = indexStart; i < indexStart+count; i++)
	{
		atom_string(listReturn+i,buf3,256);
		//post(buf3);
		POST("\tatomList[%i]: %s", i, buf3);
	}
	*/
}


/*void packFromAtoms(Pack* packReturn, ListCount count, t_atom* list)
{
	packReturn -> name = atom_getsymbol( &list[0] );
	t_int count = atom_getfloat( &list[1] );
	for(t_int i=0; i<count; i++)
	{
		PackOrAtom* pSub = 
		listAdd(packReturn -> listSub,  );

	}
}*/
ListCount packGetListCount(Pack* pack)
{
	int count = 2;
	ElementPackOrAtom* pElement=ListPackOrAtomGetFirst( & pack-> listSub);
	if(pElement)
	{
		//while(listHasNext(pack->pListSub, pElement))
		do
		{
			PackOrAtom* pPackOrAtom= pElement -> pData;
			if(pPackOrAtom->isAtom)
				count += 1;
			else
				count += packGetListCount( & pPackOrAtom->pack);
			pElement = ListPackOrAtomGetNext( & pack-> listSub, pElement);
		}
		while(pElement);
	}
	return count;
}

// constructor:
CONSTRUCTOR(test,)
{
	NEW_INSTANCE(test,x);
	x -> out = outlet_new(& x ->obj, &s_list);
	//class_addbang(testClass, testOnInput);
	return (void* )x;
}

//DESTRUCTOR(test,pThis) {}

void testOnInput(t_test* pThis)
{
	post("Hallo Welt!");
	t_atom atomList[8];
	for(int i=0; i< 8; i++)
	{
		SETFLOAT(atomList+i, 88);
	}
	PACK_FROMSYM( pack, gensym("testPack"));
		PACK_SUBATOM_FROM_NAME_NEW(pSubAtom,gensym("test"));
		ListPackOrAtomAdd( &pack.listSub, pSubAtom);

	//PackOrAtom subAtom;
	//subAtom.isAtom = 1;
	//SETSYMBOL( &subAtom.atom, gensym("Hallo"));

		//PACK_NEW(pPackSub,"subPack");

		PackOrAtom* pSub;
		pSub = (PackOrAtom* )getbytes(sizeof(PackOrAtom));
		pSub -> isAtom = 0;
		SETSYMBOL(& pSub -> pack . name, gensym("subPack"));
		ListPackOrAtomInit( & pSub -> pack . listSub);

		ListPackOrAtomAdd( &pack.listSub, pSub);
	

	int count=packGetListCount(&pack);
	POST("count: %i",count);
	atomsFromPack(count,atomList,&pack);

	t_atom testatom= atomList[3];
	char bufX[256];
	atom_string(& testatom, bufX,256);
	post(bufX);

	//freebytes(pack.pListSub,sizeof(LIST));

	outlet_list(
		pThis -> out,
		&s_list,
		count,
		atomList
	);
		ListPackOrAtomExit( & pSub -> pack . listSub);
		//freebytes(pSub,sizeof(PackOrAtom));

	ListPackOrAtomExit(& pack.listSub);
}





