#include "PX_Object_Animation.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_AnimationRender)
{
	PX_Object_Animation* pDesc = PX_ObjectGetDesc(PX_Object_Animation,pObject);
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

PX_OBJECT_FREE_FUNCTION(PX_Object_AnimationFree)
{
	PX_Object_Animation* pDesc = PX_ObjectGetDesc(PX_Object_Animation, pObject);
	if (pDesc)
		PX_AnimationFree(&pDesc->animation);
}

PX_Object* PX_Object_AnimationAttachObject( PX_Object* pObject,px_int attachIndex, px_int x, px_int y, PX_AnimationLibrary* lib)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_Animation* pDesc;
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pDesc = (PX_Object_Animation*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_ANIMATION, 0, PX_Object_AnimationRender, PX_Object_AnimationFree, 0, sizeof(PX_Object_Animation));
	if (!pDesc)
	{
		return PX_NULL;
	}
	if (lib)
	{
		PX_AnimationCreate(&pDesc->animation, lib);
	}
	pDesc->_align = PX_ALIGN_CENTER;
	return pObject;
}

PX_Object * PX_Object_AnimationCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,PX_AnimationLibrary *lib)
{
	PX_Object *pObject;
	pObject = PX_ObjectCreate(mp, Parent, (px_float)x, (px_float)y, 0, 0, 0, 0);
	if (!PX_Object_AnimationAttachObject(pObject, 0, x, y, lib))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}

px_void PX_Object_AnimationSetLibrary(PX_Object *pObject,PX_AnimationLibrary *lib)
{
	PX_Object_Animation *pDesc=PX_Object_GetAnimation(pObject);
	PX_ASSERTIF(!pDesc);//object is not a animation object
	PX_AnimationSetLibrary(&pDesc->animation,lib);
}

PX_Object_Animation * PX_Object_GetAnimation(PX_Object *pObject)
{
	px_int i;
	for (i=0;i<PX_COUNTOF(pObject->pObjectDesc);i++)
	{
		if (pObject->Type[i] ==PX_OBJECT_TYPE_ANIMATION)
		{
			return PX_ObjectGetDescIndex(PX_Object_Animation,pObject,i);
		}
	}
	return PX_NULL;
}

px_void PX_Object_AnimationSetAlign(PX_Object *panimation,PX_ALIGN Align)
{
	PX_Object_Animation *pDesc=PX_Object_GetAnimation(panimation);
	PX_ASSERTIF(!pDesc);//object is not a animation object
	pDesc->_align =Align;
}

px_void	   PX_Object_AnimationPlay(PX_Object* pObject, const px_char name[])
{
	PX_Object_Animation* pDesc = PX_Object_GetAnimation(pObject);
	PX_ASSERTIF(!pDesc);//object is not a animation object
	PX_AnimationPlay(&pDesc->animation, name);
	pDesc->state=PX_OBJECT_ANIMATION_STATE_PLAY;	
}

px_void	   PX_Object_AnimationPause(PX_Object* pObject)
{
	PX_Object_Animation* pDesc = PX_Object_GetAnimation(pObject);
	PX_ASSERTIF(!pDesc);//object is not a animation object
	pDesc->state = PX_OBJECT_ANIMATION_STATE_PAUSE;
}
px_void	   PX_Object_AnimationResume(PX_Object* pObject)
{
	PX_Object_Animation* pDesc = PX_Object_GetAnimation(pObject);
	PX_ASSERTIF(!pDesc);//object is not a animation object
	pDesc->state = PX_OBJECT_ANIMATION_STATE_PLAY;
}