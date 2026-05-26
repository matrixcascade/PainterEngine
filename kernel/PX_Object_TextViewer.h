#ifndef PX_OBJECT_TEXTVIEWER_H
#define PX_OBJECT_TEXTVIEWER_H
#include "PX_Object.h"

typedef struct 
{
	px_color TextColor;
	PX_FontModule *fontModule;
	px_string text;
	px_texture render_target;
	PX_Object* hslider, * vslider;
}PX_Object_TextViewer;

PX_Object * PX_Object_TextViewerCreate(px_memorypool *mp,PX_Object *Parent,px_int x, px_int y, px_int width,px_int height,PX_FontModule *fm);
px_void  PX_Object_TextViewerSetTextColor( PX_Object *pObject,px_color Color );
px_void  PX_Object_TextViewerSetText(PX_Object *pObject,const px_char *Text);
const px_char * PX_Object_TextViewerGetText(PX_Object *pObject);

#endif

