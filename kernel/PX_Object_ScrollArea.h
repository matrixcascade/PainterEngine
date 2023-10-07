#ifndef PX_OBJECT_SCROLLAREA_H
#define PX_OBJECT_SCROLLAREA_H
#include "PX_Object.h"


typedef struct 
{
	px_bool bBorder;
	px_surface surface;
	px_color BackgroundColor;
	px_color borderColor;
	PX_Object *root;
	PX_Object *hscroll,*vscroll;
}PX_Object_ScrollArea;


PX_Object *PX_Object_ScrollAreaCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y, px_int width, px_int height);
PX_Object_ScrollArea * PX_Object_GetScrollArea( PX_Object *pObject );
PX_Object * PX_Object_ScrollAreaGetIncludedObjects(PX_Object *pObj);
px_void PX_Object_ScrollAreaMoveToBottom(PX_Object *pObject);
px_void PX_Object_ScrollAreaMoveToTop(PX_Object *pObject);
px_void PX_Object_ScrollAreaGetRegion(PX_Object *pObject,px_float *left,px_float *top,px_float *right,px_float *bottom);
px_void PX_Object_ScrollAreaUpdateRange( PX_Object *pObject);
px_void PX_Object_ScrollAreaRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed);
px_void PX_Object_ScrollAreaSetBkColor(PX_Object *pObj,px_color bkColor);
px_void PX_Object_ScrollAreaSetBorder( PX_Object *pObj,px_bool Border );
px_void PX_Object_ScrollAreaSetBorderColor(PX_Object *pObj,px_color borderColor);
px_void PX_Object_ScrollAreaFree(PX_Object *pObj);
px_void PX_Object_ScrollAreaClear(PX_Object* pObj);
PX_Object* PX_Object_ScrollAreaSetBackgroundColor(PX_Object* pObj, px_color color);

px_color PX_Object_ScrollAreaGetBackgroundColor(PX_Object* pObj);
#endif

