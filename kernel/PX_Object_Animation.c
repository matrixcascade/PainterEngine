#include "PX_Object_Animation.h"

px_void PX_Object_AnimationRender(px_surface* psurface, PX_Object* pObject, px_uint elapsed)
{
	PX_Object_Animation* pDesc = PX_Object_GetAnimation(pObject);
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;

	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	if (pDesc&&pDesc->state==PX_OBJECT_ANIMATION_STATE_PLAY)
	{
		PX_AnimationUpdate(&pDesc->animation, elapsed);
		PX_AnimationRender(psurface, &pDesc->animation, (px_int)objx, (px_int)objy, pDesc->_align, PX_NULL);
	}

}

px_void PX_Object_AnimationFree(PX_Object* pObj)
{
	PX_Object_Animation* pDesc = PX_Object_GetAnimation(pObj);
	if (pDesc)
		PX_AnimationFree(&pDesc->animation);
}


PX_Object * PX_Object_AnimationCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,PX_AnimationLibrary *lib)
{
	PX_Object *pObject;
	PX_Object_Animation* pDesc;
	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, 0, 0, 0, PX_OBJECT_TYPE_ANIMATION, 0, PX_Object_AnimationRender, PX_Object_AnimationFree, 0, sizeof(PX_Object_Animation));
	if (pObject == PX_NULL)
	{
		return PX_NULL;
	}
	pDesc = PX_ObjectGetDesc(PX_Object_Animation, pObject);
	if (lib)
	{
		PX_AnimationCreate(&pDesc->animation, lib);
	}
	pDesc->_align =PX_ALIGN_CENTER;
	return pObject;
}

px_void PX_Object_AnimationSetLibrary(PX_Object *pObject,PX_AnimationLibrary *lib)
{
	PX_Object_Animation *pDesc=PX_Object_GetAnimation(pObject);
	if (pDesc)
	{
		PX_AnimationSetLibrary(&pDesc->animation,lib);
	}
}

PX_Object_Animation * PX_Object_GetAnimation(PX_Object *pObject)
{
	if(pObject->Type==PX_OBJECT_TYPE_ANIMATION)
		return (PX_Object_Animation *)pObject->pObjectDesc;
	else
		return PX_NULL;
}

px_void PX_Object_AnimationSetAlign(PX_Object *panimation,PX_ALIGN Align)
{
	PX_Object_Animation *pDesc=PX_Object_GetAnimation(panimation);
	if (pDesc)
	{
		pDesc->_align =Align;
	}
}

px_void	   PX_Object_AnimationPlay(PX_Object* pObject, const px_char name[])
{
	PX_Object_Animation* pDesc = PX_Object_GetAnimation(pObject);
	if (pDesc)
	{
		PX_AnimationPlay(&pDesc->animation, name);
		pDesc->state=PX_OBJECT_ANIMATION_STATE_PLAY;
	}
		
}
px_void	   PX_Object_AnimationPause(PX_Object* pObject)
{
	PX_Object_Animation* pDesc = PX_Object_GetAnimation(pObject);
	if (pDesc)
	{
		pDesc->state = PX_OBJECT_ANIMATION_STATE_PAUSE;
	}
}
px_void	   PX_Object_AnimationResume(PX_Object* pObject)
{
	PX_Object_Animation* pDesc = PX_Object_GetAnimation(pObject);
	if (pDesc)
	{
		pDesc->state = PX_OBJECT_ANIMATION_STATE_PLAY;
	}
}