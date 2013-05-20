#ifndef _SGINPUTTOFUNCTION_H_
#define _SGINPUTTOFUNCTION_H_

#include "Pack.h"
#include <stdarg.h>


//typedef sgData
/*typedef struct SDeviceCond {
	t_atom param;
	t_atom[] values;
};*/

typedef ListPack ListDeviceCond;

typedef struct SDeviceBlock {
	t_atom name;
	ListDeviceCond listDeviceCond;
} DeviceBlock;
DECL_LIST_STD(ListDeviceBlock,ElementDeviceBlock,DeviceBlock);
#define DEVICEBLOCK_NEW(VARNAME,NAME)\
	DeviceBlock* VARNAME = getbytes(sizeof(DeviceBlock));\
	VARNAME -> name = NAME;\
	DEVICEBLOCK_INIT(VARNAME)
#define DEVICEBLOCK(VAR_NAME,ATOMNAME)\
	DeviceBlock VAR_NAME;\
	VAR_NAME.name = ATOMNAME;\
	DEVICEBLOCK_INIT(VAR_NAME)
#define DEVICEBLOCK_INIT(NAME)\
	ListPackInit( & (NAME) -> listDeviceCond)
#define DEVICEBLOCK_EXIT(NAME)\
	ListPackExit( & (NAME) -> listDeviceCond)
void deviceBlockFree(DeviceBlock* device);

typedef struct SValueInformation {
	t_atom deviceParam;
	float minOut, maxOut, minIn, maxIn;
} ValueInformation;

typedef struct SMessageFragment {
	int isAtom;
	union
	{
		t_atom atom;
		ValueInformation valueInformation;
	};
} MessageFragment;
#define MESSAGEFRAGMENT_FROMATOM_NEW(VAR_NAME,pATOM)\
	MessageFragment* VAR_NAME = getbytes(sizeof(MessageFragment));\
	VAR_NAME -> isAtom = 1;\
	VAR_NAME -> atom = *(pATOM)
#define MESSAGEFRAGMENT_FROMVALUEINFORMATION_NEW(VAR_NAME,VALUEINF)\
	MessageFragment* VAR_NAME = getbytes(sizeof(MessageFragment));\
	VAR_NAME -> isAtom = 0;\
	VAR_NAME -> valueInformation = VALUEINF


#define MESSAGEFRAGMENT_FROMATOM(VAR_NAME,pATOM)\
	MessageFragment VAR_NAME;\
	VAR_NAME.isAtom = 1;\
	VAR_NAME.atom = *(pATOM)
#define MESSAGEFRAGMENT_FROMVALUEINFORMATION(VAR_NAME,VALUEINF)\
	MessageFragment VAR_NAME;\
	VAR_NAME . isAtom = 0;\
	VAR_NAME . valueInformation = VALUEINF


DECL_LIST_STD(ListMessageFragment,ElementMessageFragment,MessageFragment);

typedef struct SMessageBlock {
	t_atom name;
	ListMessageFragment listMessageFragment;
} MessageBlock;
void MessageBlockFree(MessageBlock* pMessageBlock);
#define MESSAGEBLOCK_FROMATOM_NEW(VAR_NAME,NAME)\
	MessageBlock* VAR_NAME = getbytes(sizeof(MessageBlock));\
	VAR_NAME -> name = NAME;\
	MESSAGEBLOCK_INIT(VAR_NAME)
/*#define MESSAGEBLOCK_NEW(VAR_NAME)\
	MessageBlock* VAR_NAME = getbytes(sizeof(MessageBlock));\
	MESSAGEBLOCK_INIT(VAR_NAME)*/
#define MESSAGEBLOCK_INIT(VAR_NAME)\
	ListMessageFragmentInit( & VAR_NAME -> listMessageFragment )
#define MESSAGEBLOCK_EXIT(VAR_NAME)\
	ListMessageFragmentExit( & VAR_NAME -> listMessageFragment )

DECL_LIST_STD(ListMessageBlock,ElementMessageBlock,MessageBlock);

typedef struct SIToFBlock {
	ListDeviceBlock listDevice;
	ListMessageBlock listMessage;
} IToFBlock;
void IToFBlockFree(IToFBlock* pIToFBlock);
#define ITOFBLOCK_NEW(NAME)\
	IToFBlock* NAME = NULL;\
	NAME = getbytes(sizeof(IToFBlock));\
	ITOFBLOCK_INIT(NAME)
#define ITOFBLOCK_INIT(NAME)\
	ListDeviceBlockInit( & NAME -> listDevice);\
	ListMessageBlockInit( & NAME -> listMessage);
#define ITOFBLOCK_EXIT(NAME)\
	ListDeviceBlockExit( & NAME -> listDevice);\
	ListMessageBlockExit( & NAME -> listMessage);
	

DECL_LIST_STD(ListIToF,ElementIToF,IToFBlock);

typedef struct _sgInputToFunction {
	//internal obj information:
	t_object obj;
	t_outlet* pOutlet;
	ListIToF listIToF;
	//sgData listInputToF;
} t_sgInputToFunction;

t_class* sgInputToFunctionInit();
void sgInputToFunctionExit();


CONSTRUCTOR(sgInputToFunction,);
DESTRUCTOR(sgInputToFunction,x);
void sgInputToFunction_OnInput(t_sgInputToFunction* pThis, t_symbol* sym, int argc, t_atom* argv);
void sgInputToFunction_OnSetInputToFunctionList(t_sgInputToFunction* pThis, t_symbol* sym, int argc, t_atom* argv);

// pIToF should be allocated yet!!
void parseIToF(IToFBlock* pIToF, int count, t_atom* atoms);

// vaList params: IToFBlock* pIToF
// allocates a new DeviceBlock
// fills it by parsing the "atoms"
// append the DeviceBlock to "pIToF"
void parseDeviceAndAppendToIToF(t_atom* pName, int count, t_atom* atoms, va_list vaList);

// vaList params: t_sgInputToFunction* pThis
//  parses "atoms"
// allocates a new DeviceBlock, representing the result
// append this DeviceBlock to "pThis"
void parseIToFAndAppendtoThis(t_atom* pName, int count, t_atom* atoms, va_list vaList);
// pDevice should be allocated yet!!
void parseDevice(DeviceBlock* pDevice, int count, t_atom* atoms);

typedef void (*FForPack) (t_atom* pName, int count, t_atom* atoms, va_list vaList);

typedef struct STraversalInfo
{
	t_atom* atoms;
	int count;
} TraversalInfo;

#define ALLPACKS (-1)

TraversalInfo traverseAtomsAsPacks(int atomCount, t_atom* atoms, FForPack func, int iCountPacksMax, ... );



t_atom getAtom_(DeviceBlock* pDeviceBlockInput, ValueInformation* pValInf);

#endif
