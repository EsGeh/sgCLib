#include "sgInputToFunction.h"

DEF_LIST_STD(ListDeviceBlock,ElementDeviceBlock,DeviceBlock);
DEF_LIST_STD(ListMessageFragment,ElementMessageFragment,MessageFragment);
DEF_LIST_STD(ListMessageBlock,ElementMessageBlock,MessageBlock);
DEF_LIST_STD(ListIToF,ElementIToF,IToFBlock);

static t_class* sgInputToFunctionClass;


void deviceBlockFree(DeviceBlock* pDevice)
{
	// call packFree() for every Device param
	ElementPack* pElDeviceCond = ListPackGetFirst( & pDevice -> listDeviceCond);
	while (pElDeviceCond)
	{
		Pack* pPackDeviceCond = pElDeviceCond -> pData;
		packFree(pPackDeviceCond);

		pElDeviceCond = ListPackGetNext( & pDevice -> listDeviceCond, pElDeviceCond);
	}
	ListPackExit( & pDevice -> listDeviceCond);
}
void MessageBlockFree(MessageBlock* pMessageBlock)
{
	//
	ListMessageFragmentExit(  & pMessageBlock -> listMessageFragment);
}
void IToFBlockFree(IToFBlock* pIToFBlock)
{
	//delete all Devices:
	ElementDeviceBlock* pElDevice = ListDeviceBlockGetFirst( & pIToFBlock -> listDevice);
	while (pElDevice)
	{
		DeviceBlock* pDevice = pElDevice ->pData;
		deviceBlockFree(pDevice);

		pElDevice = ListDeviceBlockGetNext( & pIToFBlock ->listDevice, pElDevice);
	}
	ListDeviceBlockExit( & pIToFBlock -> listDevice);

	// delete all messages:
	ElementMessageBlock* pElMessage = ListMessageBlockGetFirst( & pIToFBlock -> listMessage);
	while (pElMessage)
	{
		MessageBlock* pMessageBlock = pElMessage -> pData;
		MessageBlockFree(pMessageBlock);
		pElMessage = ListMessageBlockGetNext( & pIToFBlock -> listMessage, pElMessage);
	}
	ListMessageBlockExit( & pIToFBlock -> listMessage );
}

t_class* sgInputToFunctionInit()
{
	sgInputToFunctionClass = class_new(
		gensym("sgInputToFunctionC"),
		(t_newmethod )sgInputToFunctionObjNew,
		(t_method )sgInputToFunctionObjFree,
		sizeof(t_sgInputToFunction),
		CLASS_DEFAULT,
		0
	);
	class_addlist(sgInputToFunctionClass, sgInputToFunction_OnInput);
	class_addmethod(
		sgInputToFunctionClass,
		(t_method )sgInputToFunction_OnSetInputToFunctionList,
		gensym("setIToF"),
		A_GIMME
	);

	return sgInputToFunctionClass;
}
void sgInputToFunctionExit()
{
}

CONSTRUCTOR(sgInputToFunction,)
{
	post("sgInputToFunctionNew");
	NEW_INSTANCE(sgInputToFunction,x);
	/*inlet_new(
		& x -> obj,
		& x -> obj . ob_pd,
		gensym("list"),
		gensym("input")
	);*/
	inlet_new(
		& x -> obj,
		& x -> obj . ob_pd,
		gensym("list"),
		gensym("setIToF")
	);

	x -> pOutlet = outlet_new( &(x -> obj), &s_list);
	ListIToFInit( & x -> listIToF);
	return (void* )x;
}
DESTRUCTOR(sgInputToFunction,x)
{
	// free all IToFBlocks:
	POST("sgInputToFunctionFree");
	ElementIToF* pElIToF = ListIToFGetFirst( & x->listIToF);
	while (pElIToF)
	{
		IToFBlock* pIToF = pElIToF -> pData;
		IToFBlockFree(pIToF);
		pElIToF = ListIToFGetNext( & x->listIToF, pElIToF);
	}
	ListIToFExit( & x -> listIToF);
}

void sgInputToFunction_OnInput(t_sgInputToFunction* pThis, t_symbol* sym, int argc, t_atom* argv)
{
	// index of the input list
	//t_int indexCurrentAtom = 0;

	//post("on_Input");
	// 1. parse the input:
	DeviceBlock deviceBlockInput;
	DEVICEBLOCK_INIT( &deviceBlockInput);
	parseDevice(&deviceBlockInput, argc, argv);
	//onInput_parseDevice(&device,argc,argv);

	// search listIToF for the device given as input:
	// traverse through the "inputToFunction"-list
	ElementIToF* pElIToF = ListIToFGetFirst(&pThis -> listIToF);
	while (pElIToF)
	{
		//post("inputToFunction pack");
		IToFBlock* pIToF = pElIToF -> pData;
		if(deviceFound(pIToF,argc,argv))
		{
			//POST("PASSED!");
			// output all the messages of the current block:
			ElementMessageBlock* pElMessage = ListMessageBlockGetFirst( & pIToF -> listMessage );
			while (pElMessage)
			{
				MessageBlock* pMessage = pElMessage -> pData;
				t_atom atomList[128];
				atomList[0] = pMessage -> name;
				int iCurrentAtom = 2;
				ElementMessageFragment* pElMessageFrag = ListMessageFragmentGetFirst( & pMessage -> listMessageFragment);
				while (pElMessageFrag)
				{
					MessageFragment* pMessageFrag = pElMessageFrag -> pData;
					if(pMessageFrag-> isAtom)
					{
						atomList[iCurrentAtom] = pMessageFrag -> atom;
					}
					else
					{
						atomList[iCurrentAtom] = getAtom(&deviceBlockInput, & pMessageFrag -> valueInformation);
						//POST("Not an atom!");
						//atomList[iCurrentAtom] = pMessageFrag ->

					}
					iCurrentAtom ++;
					//traverseAtomsAsPacks(argc, argv, onInputFits, 1, ?);
					pElMessageFrag = ListMessageFragmentGetNext( & pMessage -> listMessageFragment, pElMessageFrag);
				}
				SETFLOAT( & atomList[1], iCurrentAtom - 2);
				outlet_list(
					pThis -> obj . ob_outlet,
					& s_list,
					iCurrentAtom,
					atomList
				);
				pElMessage = ListMessageBlockGetNext ( & pIToF -> listMessage, pElMessage);
			}
		}
		pElIToF = ListIToFGetNext( & pThis -> listIToF, pElIToF);
	}
	deviceBlockFree(& deviceBlockInput);
}

void sgInputToFunction_OnSetInputToFunctionList(t_sgInputToFunction* pThis, t_symbol* sym, int argc, t_atom* argv)
{
	/* format:
	iToF( input() message1() message2() ...)
	*/
	// to do correctly delete listIToF:
	ListIToFExit(& pThis -> listIToF);
	ListIToFInit(& pThis -> listIToF);


	post("onSetIToF");
	/*POST("argc: %i",argc);*/
	traverseAtomsAsPacks(argc, argv, parseIToFAndAppendtoThis, ALLPACKS, pThis);
}

void parseDevice(DeviceBlock* pDevice, int count, t_atom* atoms)
{
	traverseAtomsAsPacks(count, atoms, device_, 1, pDevice);
}

void device_(t_atom* pName, int count, t_atom* atoms, va_list vaList)
{
	/*t_sgInputToFunction* pThis;
	pThis = va_arg(vaList,t_sgInputToFunction*);*/
	DeviceBlock* pDevice = va_arg(vaList,DeviceBlock*);
	pDevice -> name = *pName;

	/*char buf[256];
	atom_string(pName, buf, 256);
	POST("device: %s", buf);*/

	//DEVICEBLOCK_NEW(pDevice, *pName);
	{
		//parseDevice(pDevice,count,atoms);
		traverseAtomsAsPacks(count,atoms,inputCond,ALLPACKS,pDevice);
	}
	//ListDeviceBlockAdd( & pIToF -> listDevice, pDevice);
}

void inputCond(t_atom* pName, int count, t_atom* atoms, va_list vaList)
{
	/*t_sgInputToFunction* pThis;
	pThis = va_arg(vaList,t_sgInputToFunction*);*/
	DeviceBlock* pDevice = va_arg(vaList,DeviceBlock*);

	/*char buf[256];
	atom_string(pName, buf, 256);
	POST("inputCond: %s", buf);*/

	PACK_NEW(pPackInputCond, *pName);
	{
		for(int iAtom = 0; iAtom < count; iAtom++)
		{
			PACK_ADD_ATOM(pPackInputCond, atoms[iAtom]);
		}
	}
	//PACK_ADD_PACK(pDevice, PackInputCond);
	ListPackAdd( & pDevice -> listDeviceCond, pPackInputCond);
}

BOOL deviceFound(IToFBlock* pIToF, int arg, t_atom* argv)
{
	// traverse Devices:
	ElementDeviceBlock* pElDevice = ListDeviceBlockGetFirst( & pIToF -> listDevice);
	while (pElDevice)
	{
		//POST("DEVICE");
		//unpack the device name from the input:
		DeviceBlock* pDevice = pElDevice -> pData;
		PACKHEADER_FROMATOMS(input_deviceName,input_count,&argv[0]);
		//and compare it to the current device:
		if(compareAtoms(& pDevice -> name, input_deviceName))
		{
			// traverse conditions:
			ElementPack* pElInpCond = ListPackGetFirst( & pDevice -> listDeviceCond);
			while (pElInpCond)
			{
				//POST("input cond");
				Pack* pPackInpCond = pElInpCond -> pData;
				//search for the param in the input message:
				int bInputPassed = 0;
				int input_indexAtom = 2;
				while( input_indexAtom < 2 + input_count)
				{
					//POST("input_indexAtom: %i",input_indexAtom);
					PACKHEADER_FROMATOMS(inputParam_name,inputParam_count,&argv[input_indexAtom]);
					if(compareAtoms(inputParam_name,&pPackInpCond ->name))
					if(inputParam_count == ListPackOrAtomGetSize(&pPackInpCond ->listSub))
					if( inputParam_count > 0)
					{
						int iAtom;
						ElementPackOrAtom* pElement;
						for(
							iAtom = input_indexAtom+2, pElement = ListPackOrAtomGetFirst( & pPackInpCond -> listSub);
							iAtom < input_indexAtom + 2 + inputParam_count && (compareAtoms(& argv[iAtom],& pElement -> pData -> atom ));
							iAtom ++
						);
						if( iAtom == input_indexAtom + 2 + inputParam_count)
						{
							bInputPassed = TRUE;
							break;
						}
					}
					input_indexAtom += (2 + inputParam_count);
				}
				//condition not fulfilled:
				if(!bInputPassed)
					return FALSE;
				pElInpCond = ListPackGetNext( & pDevice -> listDeviceCond, pElInpCond);

			}
			return TRUE;
		}
		pElDevice = ListDeviceBlockGetNext( & pIToF -> listDevice, pElDevice);
	}
	return FALSE;
}

t_atom getAtom(DeviceBlock* pDeviceBlockInput, ValueInformation* pValInf)
{
	t_atom ret;
	SETFLOAT(&ret, -1);
	ElementPack* pElDeviceParam = ListPackGetFirst( & pDeviceBlockInput -> listDeviceCond );
	while( pElDeviceParam )
	{
		//POST("Checking param");
		Pack* pPackDeviceParam = pElDeviceParam -> pData;
		/*char buf[256];
		atom_string(& pPackDeviceParam -> name, buf, 256);
		POST("%s",buf);*/
		if( compareAtoms ( &pPackDeviceParam -> name, & pValInf -> deviceParam) )
		{
			//quick and dirty:
			// consider the pack to have only one parameter ( <name> ( value ) )
			int countValues = ListPackOrAtomGetSize( & pPackDeviceParam -> listSub);
			if( ! countValues)
			{
				POST("input parameter has no value!");
				return ret;
			}
			PackOrAtom* pPOAValue = ListPackOrAtomGetFirst( & pPackDeviceParam -> listSub) -> pData;
			if ( ! pPOAValue -> isAtom)
			{
				POST("not an atom!");
				return ret;
			}
			float
				inMin = pValInf -> minIn,
				inMax = pValInf -> maxIn,
				outMin = pValInf -> minOut,
				outMax = pValInf -> maxOut;
			float fret = (atom_getfloat( & pPOAValue -> atom) - inMin) * (outMax - outMin) / (inMax - inMin) + outMin - inMin;

			SETFLOAT(& ret,fret);
			//POST("found right param!");
			//SETFLOAT(&ret,10);
			return ret;
		}

		pElDeviceParam = ListPackGetNext( & pDeviceBlockInput -> listDeviceCond, pElDeviceParam);
	}
	return ret;
}

/*void onOutputFits(t_atom* pName, int count, t_atom* atoms, va_list vaList)
{
}*/

void parseDeviceAndAppendToIToF(t_atom* pName, int count, t_atom* atoms, va_list vaList)
{
	/*t_sgInputToFunction* pThis;
	pThis = va_arg(vaList,t_sgInputToFunction*);*/
	IToFBlock* pIToF = va_arg(vaList,IToFBlock*);

	/*char buf[256];
	atom_string(pName, buf, 256);
	POST("device: %s", buf);*/

	DEVICEBLOCK_NEW(pDevice, *pName);
	{
		//parseDevice(pDevice,count,atoms);
		traverseAtomsAsPacks(count,atoms,inputCond,ALLPACKS,pDevice);
	}
	ListDeviceBlockAdd( & pIToF -> listDevice, pDevice);
}

void input(t_atom* pName, int count, t_atom* atoms, va_list vaList)
{
	IToFBlock* pIToF = va_arg(vaList,IToFBlock*);

	/*char buf[256];
	atom_string(pName, buf, 256);
	POST("input: %s", buf);*/
	//ElementPack* pElementPack = ListPackGetLast(& pThis -> listInputToF);
	//Pack* pPackIToF = pElementPack -> pData;
	// to do: check for correct name!
	//PACK(PackInput,*pName);
	{
		traverseAtomsAsPacks(count, atoms, parseDeviceAndAppendToIToF, ALLPACKS, pIToF);
	}
	//PACK_ADD_PACK(pPackIToF,PackInput);
	//ListPackAdd( &pThis -> listInputToF, pPackInput);
}

void message(t_atom* pName, int count, t_atom* atoms, va_list vaList)
{
	//t_sgInputToFunction* pThis = va_arg(vaList,t_sgInputToFunction*);
	IToFBlock* pIToF = va_arg(vaList,IToFBlock*);

	/*char buf[256];
	atom_string(pName, buf, 256);
	POST("message: %s", buf);*/
	MESSAGEBLOCK_FROMATOM_NEW(pMessage,*pName);
	{
		//BOOL parseNext = FALSE;
		//BOOL bracketFound = FALSE;
		int currentParam = -1;
		ValueInformation valInf;

		for(int iAtom = 0; iAtom < count; iAtom++)
		{
			t_atom* pCurrentAtom = & atoms[iAtom];
			/*if( atomEqualsString(pCurrentAtom, "#"))
			{
				//POST("HALLO");
				parseNext = TRUE;
				continue;
			}*/
			if( atomEqualsString(pCurrentAtom, "["))
			{
				currentParam = 0;
				valInf.minOut = valInf.minIn = 0;
				valInf.maxOut = valInf.maxIn = 1;

				continue;
			}
			else if( atomEqualsString(pCurrentAtom, "]"))
			{
				MESSAGEFRAGMENT_FROMVALUEINFORMATION_NEW(pMessageFrag,valInf);
				ListMessageFragmentAdd( & pMessage -> listMessageFragment, pMessageFrag );
				currentParam = -1;
				continue;
			}
			if( currentParam != -1)
			{

				/*valInf.deviceParam = *pCurrentAtom;
				valInf.minOut = valInf.minIn = 0;
				valInf.maxOut = valInf.maxIn = 1;
				MESSAGEFRAGMENT_FROMVALUEINFORMATION_NEW(pMessageFrag,valInf);
				ListMessageFragmentAdd( & pMessage -> listMessageFragment, pMessageFrag );
				parseNext = FALSE;*/
				switch(currentParam)
				{
					case 0:
						valInf.deviceParam = *pCurrentAtom;
					break;
					case 1:
						valInf.minOut = atom_getfloat(pCurrentAtom);
					break;
					case 2:
						valInf.maxOut = atom_getfloat(pCurrentAtom);
					break;
					case 3:
						valInf.minIn = atom_getfloat(pCurrentAtom);
					break;
					case 4:
						valInf.maxIn = atom_getfloat(pCurrentAtom);
					break;
				};
				currentParam ++;
			}
			else
			{
				//POST("BLA");
				//PACK_ADD_ATOM(PackMessage,atoms[iAtom]);
				MESSAGEFRAGMENT_FROMATOM_NEW(pMessageFrag,&atoms[iAtom]);
				ListMessageFragmentAdd( & pMessage -> listMessageFragment, pMessageFrag);
			}
		}
	}
	//PACK_ADD_PACK(pPackIToF,pPackMessage);
	ListMessageBlockAdd( & pIToF->listMessage, pMessage);
}

void parseIToF(IToFBlock* pIToF, int count, t_atom* atoms)
{
	TraversalInfo travInf = traverseAtomsAsPacks(count,atoms,input,1,pIToF);

	//POST("atoms: %i", travInf.atoms - atoms);
	//POST("count: %i", travInf.count);
	traverseAtomsAsPacks(travInf.count, travInf.atoms, message, ALLPACKS, pIToF);
}

void parseIToFAndAppendtoThis(t_atom* pName, int count, t_atom* atoms, va_list vaList)
{
	t_sgInputToFunction* pThis;
	pThis = va_arg(vaList,t_sgInputToFunction*);

	/*char buf[256];
	atom_string(pName, buf, 256);
	POST("iToF: %s", buf);*/

	ITOFBLOCK_NEW(pIToF);
	//PACK_NEW(pPackIToF,*pName);

	//t_atom* atomsBefore= atoms;
	parseIToF(pIToF,count,atoms);
	/*TraversalInfo travInf = traverseAtomsAsPacks(count,atoms,input,1,pThis,pIToF);

	//POST("atoms: %i", travInf.atoms - atoms);
	//POST("count: %i", travInf.count);
	traverseAtomsAsPacks(travInf.count, travInf.atoms, message, ALLPACKS, pThis,pIToF);*/

	ListIToFAdd(& pThis -> listIToF,pIToF);
}

TraversalInfo traverseAtomsAsPacks(int atomCount, t_atom* atoms, FForPack func, int iCountPacksMax, ... )
{
	int iCurrentAtom = 0;
	int iIndexPack = 0;
	while(
		(iCurrentAtom < atomCount)
		&& ((iIndexPack < iCountPacksMax) || (iCountPacksMax == ALLPACKS))
	)
	{
		//POST("iIndexPack: %i",iCurrentAtom);
		PACKHEADER_FROMATOMS(pack_name, pack_count, &atoms[iCurrentAtom]);

		va_list vaList;
		va_start(vaList, iCountPacksMax);
		func(pack_name, pack_count, &atoms[iCurrentAtom+2], vaList);
		va_end(vaList);

		iCurrentAtom += ( 2 + pack_count );
		iIndexPack ++;
	}
	TraversalInfo travInf;
	travInf.atoms =  & atoms[iCurrentAtom];
	travInf.count = atomCount - iCurrentAtom;
	return travInf;
	//(*atomCount) -= iCurrentAtom;
	//(*atoms) += (*atomCount);
	//return & (*atoms)[iCurrentAtom];
}
