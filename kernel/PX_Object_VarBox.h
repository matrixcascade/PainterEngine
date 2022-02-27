#ifndef PX_OBJECT_VARBOX_H
#define PX_OBJECT_VARBOX_H
#include "PX_Object.h"


typedef struct
{
	PX_Object	*widget;
	PX_Object* various_name;
	PX_Object* various_address;
	PX_Object* various_size;
	PX_Object* selectbar_type;
	PX_Object* checkbox_readonly;
	PX_Object	*btn_close;
	PX_Object   *btn_ok;
}PX_Object_VarBox;

PX_Object_VarBox* PX_Object_GetVarBox( PX_Object *Object );
PX_Object * PX_Object_VarBoxCreate(px_memorypool *mp,PX_Object *Parent,int x,int y,int width,int height,const px_char title[],PX_FontModule *fontmodule);
px_void PX_Object_VarBoxShow(PX_Object *pObject);
px_void PX_Object_VarBoxClose(PX_Object* pObject);
#endif

