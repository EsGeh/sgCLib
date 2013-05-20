#include "sgInputC.h"

#include "sgDevice.h"
#include "Pack.h"
#include "sgInputToFunction.h"


void sgInputC_setup()
{
	deviceInit();
	testInit();
	sgInputToFunctionInit();
}
