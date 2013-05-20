#include "sgDevice.h"
/* output format
	sgDevice.<descr>
	descr ::=
		trigger <0..5>
		switch <0..5> <bool>
		analog <0..5> <analogVal>
		meta <0..2> <bool>
*/

static t_class* sgDevice_class;

t_class* deviceInit()
{
	sgDevice_class = class_new(
		gensym("sgDeviceC"), 		// name
		(t_newmethod )sgDeviceObjNew, 	// newmethod
		//(t_method )sgDeviceFree, 	// freemethod
		0,
		sizeof(t_sgDevice), 		// size
		CLASS_DEFAULT, 			// object type
		0 				// list of atoms following the selector...
	);
	class_addfloat(sgDevice_class, sgDeviceOnInput);
	return sgDevice_class;
}

void deviceExit()
{
}

// the constructor
void* sgDeviceObjNew(
	PARAMETERS
)
{
	t_sgDevice* x = (t_sgDevice* )pd_new(sgDevice_class);
	//x -> controlMessage = 0;
	x -> bufferLength = -1;
	x -> outTriggers = outlet_new(& x->obj, &s_list);
	x -> outSwitches = outlet_new(& x->obj, &s_list);
	x -> outAnalog = outlet_new(& x->obj, &s_list);
	x -> outMeta = outlet_new(&x->obj, &s_list);
	x -> outMidi = outlet_new(& x->obj, &s_list);
	//post("opening usb device...");
	//TRY(!usbOpen(),MSG_COULD_NOT_OPEN);
	return (void* )x;
}

/*DESTRUCTOR(sgDevice,pThis)
{
	//post("closing usb device...");
	//TRY(!usbClose(),MSG_COULD_NOT_CLOSE);
}*/

void sgDeviceOnBang(t_sgDevice* pThis)
{
	post("reconnecting:...");
	/*
	TRY(!usbClose(),MSG_COULD_NOT_CLOSE);
	TRY(!usbOpen(),MSG_COULD_NOT_OPEN);
	*/
}

void sgDeviceOnInput(t_sgDevice* pThis, t_floatarg f) 
{
	// start if status byte, or if we have yet started buffering:
	if(
		(f>=0x80)         // >= 0x80 says that its a status byte
		&& (f != 0xF7 )	// BUT ONLY IF NOT A sysex End Byte (0xF7)!!
	)
	{
		pThis->buffer[0]=f;
		pThis->bufferLength=1;
		//post("start");
	}
	else if(pThis->bufferLength > 0)
	{
		appendToBuffer(pThis,f);
		t_int* buffer=pThis->buffer;
		//find out, if the message is complete:
		if( //2 byte messages
			IN_RANGE(buffer[0],0x80,0x8f) // note on (2 data bytes)
			|| IN_RANGE( buffer[0],0x90,0x9f) // note off (2 data bytes)
			|| IN_RANGE( buffer[0],0xA0,0xAf) // aftertouch (2 data bytes)
			|| IN_RANGE( buffer[0],0xB0,0xBf) // controller (2 data bytes)
			|| IN_RANGE( buffer[0],0xE0,0xEf) // pitch wheel (2 data bytes)
		)
		{
			//post("first branch");
			if(pThis->bufferLength == 3)
			{
				outputMidi(pThis, pThis->bufferLength, buffer);
				pThis->bufferLength= -1;
			}
		}
		else if( //1 byte messages:
			IN_RANGE( buffer[0],0xC0,0xCf) // program change (1 data byte)
			|| IN_RANGE( buffer[0],0xD0,0xDf) // channel pressure (1 data byte)
		)
		{
			//post("second branch");
			if(pThis->bufferLength == 2)
			{
				outputMidi(pThis, pThis->bufferLength, buffer);
				pThis->bufferLength= -1;
			}
		}
		else if( // sysex message (any number of bytes):
			(buffer[0] ==0xF0)
			&& (f == 0xF7)
		)
		{
			/*char temp[64];
			sprintf(temp, "buffer[1] = %i, count= %i",(int )buffer[1], (int )pThis->bufferLength);
			post(temp);*/
			if (IS_TRIGGER(buffer[1]))
			{
				outputTrigger(pThis,INDEX_TRIGGER(buffer[1]));
			}
			else if (IS_SWITCH(buffer[1]))
			{
				outputSwitch(pThis,INDEX_SWITCH(buffer[1]), buffer[2]);
			}
			else if (IS_ANALOG(buffer[1]))
			{
				outputAnalog(pThis,INDEX_ANALOG(buffer[1]), buffer[2]);
			}
			else if (IS_META(buffer[1]))
			{
				outputMeta(pThis,INDEX_META(buffer[1]));
			}
			else
			{
				post("unknown control input!");
			}
			pThis->bufferLength= -1;
		}
	}
}

void appendToBuffer(t_sgDevice* pThis, t_floatarg f) 
{
	pThis->buffer[pThis->bufferLength]=f;
	pThis->bufferLength++;
}

void outputTrigger(t_sgDevice* pThis, unsigned int index)
{
	//post("index ="); 
	//sgDevice.trigger ( index ( i ) )

	t_atom list[5];
	SETSYMBOL(&list[0], gensym("sgDevice.trigger"));
	SETFLOAT(&list[1], 3);
	SETSYMBOL(&list[2], gensym("index"));
	SETFLOAT(&list[3], 1);
	SETFLOAT(&list[4], index);
	
	outlet_list(
		pThis->outTriggers,
		&s_list,
		5,
		list
	);
}
void outputSwitch(t_sgDevice* pThis, unsigned int index, unsigned int val)
{
	//sgDevice.switch ( index ( i ) value ( val ) )
	t_atom list[8];
	SETSYMBOL(&list[0], gensym("sgDevice.trigger"));
	SETFLOAT(&list[1], 6);
	SETSYMBOL(&list[2], gensym("index"));
	SETFLOAT(&list[3], 1);
	SETFLOAT(&list[4], index);
	SETSYMBOL(&list[5], gensym("value"));
	SETFLOAT(&list[6], 1);
	SETFLOAT(&list[7], val);
	
	outlet_list(
		pThis->outSwitches,
		&s_list,
		8,
		list
	);
}

void outputAnalog(t_sgDevice* pThis, unsigned int index,unsigned int val)
{
	//sgDevice.analog ( index ( i ) value ( val ) )
	/*char strBuf[256];
	sprintf(strBuf, "outputAnalog(,%i,%i)",index,val);
	post(strBuf);*/
	t_atom list[8];
	SETSYMBOL(&list[0], gensym("sgDevice.analog"));
	SETFLOAT(&list[1], 6);
	SETSYMBOL(&list[2], gensym("index"));
	SETFLOAT(&list[3], 1);
	SETFLOAT(&list[4], index);
	SETSYMBOL(&list[5], gensym("value"));
	SETFLOAT(&list[6], 1);
	SETFLOAT(&list[7], ((float )val)/127.0);
	
	outlet_list(
		pThis->outAnalog,
		&s_list,
		8,
		list
	);
}
void outputMeta(t_sgDevice* pThis, unsigned int index)
{
	//sgDevice.meta ( index ( i ) )

	t_atom list[5];
	SETSYMBOL(&list[0], gensym("sgDevice.meta"));
	SETFLOAT(&list[1], 3);
	SETSYMBOL(&list[2], gensym("index"));
	SETFLOAT(&list[3], 1);
	SETFLOAT(&list[4], index);
	
	outlet_list(
		pThis->outMeta,
		&s_list,
		5,
		list
	);

}
void outputMidi(t_sgDevice* pThis, unsigned int count, t_int* buffer )
{
	post("outputMidi");
	//POST("count: %i",count);
	t_atom* list;
	list= getbytes(count*sizeof(t_atom));
	int i;
	for(i=0; i<count; i++)
	{
		//POST("buffer[%i] = %i",i,(int )buffer[i]);
		SETFLOAT(&list[i],buffer[i]);
	}
	
	outlet_list(
		pThis->outMidi,
		&s_list,
		count,
		list
	);
	freebytes(list,count*sizeof(t_atom));
}

/*
int fd_usb=0;
char* deviceName="/dev/ttyS0";

int usbOpen()
{
	fd_usb = open(deviceName,O_RDWR | O_NOCTTY | O_NDELAY );
	if(fd_usb==-1)
		return -1;
	struct termios usbConfig;
	TRY_RET(!isatty(fd_usb),"not a tty device!",-1);
	TRY_RET(tcgetattr(fd_usb,&usbConfig)<0,"error while reading usb config!",-1);
	return 0;
}

int usbClose()
{
	return close(fd_usb);
}

int usbSend()
{
	return 1;
}

void onReceiveData()
{
}
*/
