#ifndef _SGDEVICE_H_
#define _SGDEVICE_H_

#include "Global.h"

#include "m_pd.h"
#include <fcntl.h> // working with file descriptors (open,close, ...)
#include <unistd.h>
#include <termios.h> // usb specific stuff
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#define PARAMETERS 
#define MAX_LENGTH_MESSAGE 8
#define SYSEX_START 0xf0
#define SYSEX_END 0xf7

#define TRIGGERS 0
#define SWITCHES 10
#define ANALOG 20
#define META 64

#define IS_TRIGGER(ind) IN_RANGE(ind,TRIGGERS,TRIGGERS+5)
#define IS_SWITCH(ind) IN_RANGE(ind,SWITCHES,SWITCHES+5)
#define IS_ANALOG(ind) IN_RANGE(ind,ANALOG,ANALOG+5)
#define IS_META(ind) IN_RANGE(ind,META,META+2)


#define INDEX_TRIGGER(pos) (pos-TRIGGERS)
#define INDEX_SWITCH(pos) (pos-SWITCHES)
#define INDEX_ANALOG(pos) (pos-ANALOG)
#define INDEX_META(pos) (pos-META)

#define MSG_COULD_NOT_OPEN "unable to open usb port!"
#define MSG_COULD_NOT_CLOSE "unable to close usb port!"



typedef struct _sgDevice {
	//internal obj information:
	t_object obj;
	//t_inlet* inFromUSB;
	//t_int controlMessage //current message is a control (trigger,switch, ...)
	t_int bufferLength; // number of symbols received yet
	t_int buffer[MAX_LENGTH_MESSAGE];
	// outlets:
	t_outlet* outTriggers;
	t_outlet* outSwitches;
	t_outlet* outAnalog;
	t_outlet* outMeta;
	t_outlet* outMidi;
} t_sgDevice;


/*
int usbOpen();
int usbClose();
int usbSend();
void onReceiveData();
*/

//static t_class* sgDevice_class;

t_class* deviceInit();
void deviceExit();


CONSTRUCTOR(sgDevice,);
//DESTRUCTOR(sgDevice,pThis);
void sgDeviceOnBang(t_sgDevice* pThis);
void sgDeviceOnInput(t_sgDevice* pThis, t_floatarg f);

void outputTrigger(t_sgDevice* pThis, unsigned int index);
void outputSwitch(t_sgDevice* pThis, unsigned int index, unsigned int val);
void outputAnalog(t_sgDevice* pThis, unsigned int index,unsigned int val);
void outputMeta(t_sgDevice* pThis, unsigned int index);
void outputMidi(t_sgDevice* pThis, unsigned int count, t_int* buffer );

void appendToBuffer(t_sgDevice* pThis, t_floatarg f);

#endif
