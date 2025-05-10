#ifndef PX_OBJECT_STAMP_H
#define PX_OBJECT_STAMP_H

#include "PX_Object.h"


typedef struct  
{
	px_dword elapsed;
	px_texture* ptexture;
}PX_Object_Stamp;


PX_Object* PX_Object_StampCreate(px_memorypool* mp, PX_Object* Parent,px_float x,px_float y);
px_void PX_Object_StampPresent(PX_Object* pObject, px_texture* ptexture);
#endif