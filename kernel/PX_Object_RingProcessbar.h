#ifndef PX_OBJECT_RINGPROCESSBAR_H
#define PX_OBJECT_RINGPROCESSBAR_H
#include "PX_Object.h"

typedef struct 
{
	px_int Value;
	px_int Min;
	px_int Max;
	px_int linewidth;
	px_color Color;
	px_color BackgroundColor;
	PX_FontModule *fm;
}PX_Object_RingProcessBar;

PX_Object * PX_Object_RingProcessBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int r,PX_FontModule *fm);
PX_Object_RingProcessBar *PX_Object_GetRingProcessBar(PX_Object *Object);
px_void		PX_Object_RingProcessBarSetColor(PX_Object *pRingProcessBar,px_color Color);
px_void		PX_Object_RingProcessBarSetBackgroundColor(PX_Object *pRingProcessBar,px_color Color);
px_void		PX_Object_RingProcessBarSetValue(PX_Object *pRingProcessBar,px_int Value);
px_void		PX_Object_RingProcessBarSetMax(PX_Object *pRingProcessBar,px_int Max);
px_int		PX_Object_RingProcessBarGetMax(PX_Object* pRingProcessBar);
px_int		PX_Object_RingProcessBarGetValue(PX_Object* pRingProcessBar);


#endif

