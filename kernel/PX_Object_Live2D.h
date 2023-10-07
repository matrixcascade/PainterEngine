#ifndef PX_OBJECT_LIVE2D_H
#define PX_OBJECT_LIVE2D_H
#include "PX_Object.h"
#include "PX_LiveFramework.h"

typedef struct 
{
	PX_Live live;
}PX_Object_Live2D;

PX_Object* PX_Object_Live2DCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, PX_LiveFramework *pLiveFramework);
px_void PX_Object_Live2DPlayAnimation(PX_Object *pObject,px_char *name);
px_void PX_Object_Live2DPlayAnimationRandom(PX_Object* pObject);
px_void PX_Object_Live2DPlayAnimationIndex(PX_Object* pObject, px_int index);
#endif



