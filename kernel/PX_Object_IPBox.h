#ifndef PX_OBJECT_IPBOX_H
#define PX_OBJECT_IPBOX_H
#include "PX_Object.h"


typedef struct
{
	PX_Object	*widget;
	PX_Object* various_ip;
	PX_Object* various_port;
	PX_Object	*btn_close;
	PX_Object   *btn_ok;
}PX_Object_IPBox;

PX_Object_IPBox* PX_Object_GetIPBox( PX_Object *Object );
PX_Object * PX_Object_IPBoxCreate(px_memorypool *mp,PX_Object *Parent,int x,int y,int width,int height,const px_char title[],PX_FontModule *fontmodule);
px_void PX_Object_IPBoxShow(PX_Object *pObject);
px_void PX_Object_IPBoxClose(PX_Object* pObject);
px_void PX_Object_IPBoxSetDefault(PX_Object* pObject, const px_char ip[], const px_char port[]);
#endif

