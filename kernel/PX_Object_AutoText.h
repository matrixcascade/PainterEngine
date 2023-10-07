#ifndef PX_OBJECT_AUTOTEXT_H
#define PX_OBJECT_AUTOTEXT_H
#include "PX_Object.h"

typedef struct 
{
	px_color TextColor;
	PX_FontModule *fontModule;
	px_string text;
}PX_Object_AutoText;


PX_Object *PX_Object_AutoTextCreate(px_memorypool *mp,PX_Object *Parent,px_int x, px_int y, px_int limit_width,PX_FontModule *fm);
PX_Object_AutoText * PX_Object_GetAutoText( PX_Object *pObject );
px_void PX_Object_AutoTextSetTextColor( PX_Object *pObject,px_color Color );
px_void PX_Object_AutoTextSetText(PX_Object *pObject,const px_char *Text);
const px_char * PX_Object_AutoTextGetText(PX_Object *pObject);
px_void PX_Object_AutoTextRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed);
px_void PX_Object_AutoTextFree(PX_Object *pObject);

PX_Designer_ObjectDesc PX_Object_AutoTextDesignerInstall();

#endif

