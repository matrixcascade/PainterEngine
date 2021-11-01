#ifndef PX_OBJECT_AUTOTEXT_H
#define PX_OBJECT_AUTOTEXT_H
#include "PX_Object.h"

typedef struct 
{
	px_color TextColor;
	PX_FontModule *fontModule;
	px_string text;
}PX_Object_AutoText;


PX_Object *PX_Object_AutoTextCreate(px_memorypool *mp,PX_Object *Parent,int x,int y,int limit_width,PX_FontModule *fm);
PX_Object_AutoText * PX_Object_GetAutoText( PX_Object *Object );
px_void PX_Object_AutoTextSetTextColor( PX_Object *pObject,px_color Color );
px_void PX_Object_AutoTextSetText(PX_Object *Obj,const px_char *Text);
px_void PX_Object_AutoTextRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed);
px_void PX_Object_AutoTextFree(PX_Object *Obj);
px_int PX_Object_AutoTextGetHeight(PX_Object *Obj);


#endif

