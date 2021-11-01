#ifndef PX_OBJECT_PROCESSBAR_H
#define PX_OBJECT_PROCESSBAR_H
#include "PX_Object.h"

typedef struct 
{
	px_int Value;
	px_int MAX;
	px_color Color;
	px_color BackgroundColor;
}PX_Object_ProcessBar;

PX_Object *	PX_Object_ProcessBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height);
PX_Object_ProcessBar *PX_Object_GetProcessBar(PX_Object *Object);
px_void		PX_Object_ProcessBarSetColor(PX_Object *pProcessBar,px_color Color);
px_void		PX_Object_ProcessBarSetBackgroundColor(PX_Object *pProcessBar,px_color Color);
px_void		PX_Object_ProcessBarSetValue(PX_Object *pProcessBar,px_int Value);
px_void		PX_Object_ProcessBarSetMax(PX_Object *pProcessBar,px_int Max);
px_int		PX_Object_ProcessBarGetMax(PX_Object* pProcessBar);
px_int		PX_Object_ProcessBarGetValue(PX_Object* pProcessBar);

#endif

