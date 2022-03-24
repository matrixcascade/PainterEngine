#ifndef PX_OBJECT_COUNTERDOWN_H
#define PX_OBJECT_COUNTERDOWN_H

#include "PX_Object.h"


typedef struct  
{
	px_int ms_down;
	PX_FontModule* numericFM;
	px_int ring_ms;
	px_color fontcolor,backgroundcolor,ringcolor;

}PX_Object_CounterDown;


PX_Object* PX_Object_CounterDownCreate(px_memorypool* mp, PX_Object* Parent,px_float x,px_float y,px_float radius,PX_FontModule *fm);
PX_Object_CounterDown* PX_Object_GetCounterDown(PX_Object* pObject);
px_void PX_Object_CounterDownSetValue(PX_Object* pObject, px_int ms);
px_int PX_Object_CounterDownGetValue(PX_Object* pObject);

px_void PX_Object_CounterDownSetFontColor(PX_Object* pObject, px_color clr);
px_void PX_Object_CounterDownSetBackgroundColor(PX_Object* pObject, px_color clr);
px_void PX_Object_CounterDownSetRingColor(PX_Object* pObject, px_color clr);
#endif