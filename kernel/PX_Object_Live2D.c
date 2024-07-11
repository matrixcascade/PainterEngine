#include "PX_Object_Live2D.h"

PX_Object_Live2D * PX_Object_GetLive2D(PX_Object *pObject)
{
	PX_Object_Live2D*pdesc=(PX_Object_Live2D *)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_LIVE2D);
	PX_ASSERTIF(pdesc==PX_NULL);
	return pdesc;
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_PX_Live2DRender)
{
	PX_Object_Live2D *pLive2D=PX_ObjectGetDesc(PX_Object_Live2D,pObject);
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	PX_LiveRender(psurface,&pLive2D->live,(px_int)objx,(px_int)objy,PX_ALIGN_CENTER,elapsed);
}

PX_Object* PX_Object_Live2DAttachObject( PX_Object* pObject,px_int attachIndex, px_int x, px_int y, PX_LiveFramework* pLiveFramework)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_Live2D* pLive2D;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pLive2D = (PX_Object_Live2D*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_LIVE2D, 0, PX_Object_PX_Live2DRender, 0, 0, sizeof(PX_Object_Live2D));
	PX_ASSERTIF(pLive2D == PX_NULL);

	if (!PX_LiveCreate(mp, pLiveFramework, &pLive2D->live))
	{
		return PX_NULL;
	}
	return pObject;
}

PX_Object* PX_Object_Live2DCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, PX_LiveFramework* pLiveFramework)
{
	PX_Object* pObject;
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);
	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}
	if(!PX_Object_Live2DAttachObject(pObject,0,0,0,pLiveFramework))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}

px_void PX_Object_Live2DPlayAnimation(PX_Object* pObject, px_char* name)
{
	PX_Object_Live2D *pLive2D= PX_Object_GetLive2D(pObject);
	PX_LivePlayAnimationByName(&pLive2D->live, name);
}

px_void PX_Object_Live2DPlayAnimationIndex(PX_Object* pObject, px_int index)
{
	PX_Object_Live2D *pLive2D= PX_Object_GetLive2D(pObject);
	PX_LivePlayAnimation(&pLive2D->live, index);
}

px_void PX_Object_Live2DPlayAnimationRandom(PX_Object* pObject)
{
	PX_Object_Live2D* pLive2D = PX_Object_GetLive2D(pObject);
	px_int c = PX_LiveGetAnimationCount(&pLive2D->live);
	if (c)
	{
		PX_LivePlayAnimation(&pLive2D->live, PX_rand() % c);
	}
}