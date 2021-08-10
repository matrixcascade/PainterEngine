#include "PX_Object_Animation.h"

PX_Object * PX_Object_AnimationCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,PX_AnimationLibrary *lib)
{
	PX_Object *pObject;
	PX_Object_Animation *pAnimation=(PX_Object_Animation *)MP_Malloc(mp,sizeof(PX_Object_Animation));
	if (pAnimation==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,0,0,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pAnimation);
		return PX_NULL;
	}

	pObject->pObject=pAnimation;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_ANIMATION;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_Object_AnimationFree;
	pObject->Func_ObjectRender=PX_Object_AnimationRender;
	PX_AnimationCreate(&pAnimation->animation,lib);
	pAnimation->Align=PX_ALIGN_CENTER;
	return pObject;
}

px_void PX_Object_AnimationSetLibrary(PX_Object *Object,PX_AnimationLibrary *lib)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(Object);
	if (pA)
	{
		PX_AnimationSetLibrary(&pA->animation,lib);
	}
}

PX_Object_Animation * PX_Object_GetAnimation(PX_Object *Object)
{
	if(Object->Type==PX_OBJECT_TYPE_ANIMATION)
		return (PX_Object_Animation *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_AnimationSetAlign(PX_Object *panimation,PX_ALIGN Align)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(panimation);
	if (pA)
	{
		pA->Align=Align;
	}
}

px_void PX_Object_AnimationRender(px_surface *psurface,PX_Object *pObject,px_uint elapsed)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pA)
	{
		PX_AnimationUpdate(&pA->animation,elapsed);

		switch(pA->Align)
		{
		case PX_ALIGN_LEFTTOP:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)objx,(px_int)objy,PX_ALIGN_LEFTTOP,PX_NULL);
			}
			break;
		case PX_ALIGN_LEFTMID:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)objx,(px_int)(objy+objHeight/2),PX_ALIGN_LEFTMID,PX_NULL);
			}
			break;
		case PX_ALIGN_LEFTBOTTOM:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)objx,(px_int)(objy+objHeight),PX_ALIGN_LEFTBOTTOM,PX_NULL);
			}
			break;
		case PX_ALIGN_MIDTOP:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth/2),(px_int)(objy),PX_ALIGN_MIDTOP,PX_NULL);
			}
			break;
		case PX_ALIGN_CENTER:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth/2),(px_int)(objy+objHeight/2),PX_ALIGN_CENTER,PX_NULL);
			}
			break;
		case PX_ALIGN_MIDBOTTOM:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth/2),(px_int)(objy+objHeight),PX_ALIGN_MIDBOTTOM,PX_NULL);
			}
			break;
		case PX_ALIGN_RIGHTTOP:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth),(px_int)(objy),PX_ALIGN_RIGHTTOP,PX_NULL);
			}
			break;
		case PX_ALIGN_RIGHTMID:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth),(px_int)(objy+objHeight/2),PX_ALIGN_RIGHTMID,PX_NULL);
			}
			break;
		case PX_ALIGN_RIGHTBOTTOM:
			{
				PX_AnimationRender(psurface,&pA->animation,(px_int)(objx+objWidth),(px_int)(objy+objHeight),PX_ALIGN_RIGHTBOTTOM,PX_NULL);
			}
			break;
		}
	}

}

px_void PX_Object_AnimationFree(PX_Object *pObj)
{
	PX_Object_Animation *pA=PX_Object_GetAnimation(pObj);
	if (pA)
		PX_AnimationFree(&pA->animation);
}
