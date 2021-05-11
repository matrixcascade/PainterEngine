#ifndef PX_OBJECT_ANIMATION_H
#define PX_OBJECT_ANIMATION_H
#include "PX_Object.h"


PX_Object *PX_Object_AnimationCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,PX_Animationlibrary *lib);
px_void PX_Object_AnimationSetLibrary(PX_Object *Object,PX_Animationlibrary *lib);
PX_Object_Animation *PX_Object_GetAnimation(PX_Object *Object);
px_void	   PX_Object_AnimationSetAlign(PX_Object *pObject,px_dword Align);
px_void	   PX_Object_AnimationRender(px_surface *psurface,PX_Object *pImage,px_uint elpased);
px_void	   PX_Object_AnimationFree(PX_Object *pObject);


#endif