#ifndef PX_MODULE_SERIALPORT_H
#define PX_MODULE_SERIALPORT_H

#include "PX_Object.h"

#define PX_INSTANCE_SERIALPORTCONFIGURARION_NEXT 0x1000

typedef struct  
{
	px_char com[16];
	px_uint baudRate;
	px_uint DataBits;/*5,6,7,8*/
	px_char ParityType;/*'O','E','N'*/
	px_int  stopBit;/*0,1,2*/
}PX_Object_COMBox_Return;

typedef struct
{
	PX_Object *widget;
	PX_FontModule *fm;
	PX_Object *various_com,* various_baudrate,* various_databits,* various_stopbits,* various_parity;
	PX_Object *btn_ok,*btn_close;
}PX_Object_COMBox;

PX_Object_COMBox* PX_Object_GetCOMBox(PX_Object* Object);
PX_Object* PX_Object_COMBoxCreate(px_memorypool* mp, PX_Object* Parent, int x, int y, int width, int height, const px_char title[], PX_FontModule* fontmodule);
px_void PX_Object_COMBoxShow(PX_Object* pObject);
px_void PX_Object_COMBoxClose(PX_Object* pObject);
#endif