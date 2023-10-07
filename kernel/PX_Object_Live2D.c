#include "PX_Object_Live2D.h"

px_void PX_Object_PX_Live2DRender(px_surface* psurface, PX_Object* pObject, px_uint elapsed)
{
	PX_Object_Live2D *pLive2D=(PX_Object_Live2D *)pObject->pObjectDesc;
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	PX_LiveRender(psurface,&pLive2D->live,(px_int)objx,(px_int)objy,PX_ALIGN_CENTER,elapsed);
}

PX_Object* PX_Object_Live2DCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, PX_LiveFramework* pLiveFramework)
{
	PX_Object* pObject;
	PX_Object_Live2D *pLive2D;
	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,0,0,0,PX_OBJECT_TYPE_LIVE2D,0, PX_Object_PX_Live2DRender,0,0,sizeof(PX_Object_Live2D));
	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}
	pLive2D=(PX_Object_Live2D *)pObject->pObjectDesc;
	if(!PX_LiveCreate(mp,pLiveFramework,&pLive2D->live))
	{
		return PX_NULL;
	}
	return pObject;
}

px_void PX_Object_Live2DPlayAnimation(PX_Object* pObject, px_char* name)
{
	PX_Object_Live2D *pLive2D=(PX_Object_Live2D *)pObject->pObjectDesc;
	if (pObject->Type== PX_OBJECT_TYPE_LIVE2D)
	{
		PX_LivePlayAnimationByName(&pLive2D->live, name);
	}
}

px_void PX_Object_Live2DPlayAnimationIndex(PX_Object* pObject, px_int index)
{
	PX_Object_Live2D *pLive2D=(PX_Object_Live2D *)pObject->pObjectDesc;
	if (pObject->Type== PX_OBJECT_TYPE_LIVE2D)
	{
		PX_LivePlayAnimation(&pLive2D->live, index);
	}
}

px_void PX_Object_Live2DPlayAnimationRandom(PX_Object* pObject)
{
	PX_Object_Live2D* pLive2D = (PX_Object_Live2D*)pObject->pObjectDesc;
	if (pObject->Type == PX_OBJECT_TYPE_LIVE2D)
	{
		px_int c= PX_LiveGetAnimationCount(&pLive2D->live);
		if (c)
		{
			PX_LivePlayAnimation(&pLive2D->live, PX_rand()%c);
		}
		
	}
}