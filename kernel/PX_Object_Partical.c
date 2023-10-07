#include "PX_Object_Partical.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_ParticalRender)
{
	PX_Object_Partical *pDesc = (PX_Object_Partical *)pObject->pObjectDesc;
	px_float objx, objy, objWidth, objHeight;
	px_float inheritX, inheritY;
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	PX_ParticalLauncherSetPosition(&pDesc->launcher, objx, objy, 0);
	PX_ParticalLauncherRender(psurface, &pDesc->launcher, elapsed);
}

PX_Object* PX_Object_ParticalCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, PX_ParticalLauncher_InitializeInfo ParticalInfo)
{
	PX_Object *pObject;
	PX_Object_Partical *pDesc;
	pObject=PX_ObjectCreateEx(mp,Parent,x*1.0f,y*1.0f,0,0,0,0,PX_OBJECT_TYPE_PARTICAL,0,PX_Object_ParticalRender,PX_NULL,0,sizeof(PX_Object_Partical));
	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}
	pDesc=(PX_Object_Partical *)pObject->pObjectDesc;
	PX_ParticalLauncherInitialize(&pDesc->launcher, mp, ParticalInfo);

	return pObject;
}
