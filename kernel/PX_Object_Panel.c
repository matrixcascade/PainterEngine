#include "PX_Object_Panel.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_PanelRender)
{
	PX_Object_Panel *pDesc=PX_ObjectGetDesc(PX_Object_Panel,pObject);
	px_int x,y,w,h;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	w=(px_int)pObject->Width;
	h=(px_int)pObject->Height;


	if (w != pDesc->rendersurface.width || h != pDesc->rendersurface.height)
	{
		px_texture newtex;
		if (w > 0 && h > 0)
		{
			if (!PX_TextureCreate(pObject->mp, &newtex, w, h))
			{
				return;
			}
			PX_TextureCover(&newtex, &pDesc->rendersurface, 0, 0, PX_ALIGN_LEFTTOP);
			PX_TextureFree(&pDesc->rendersurface);
			pDesc->rendersurface=newtex;
		}
	}

	if (w > 0 && h > 0)
		PX_TextureRender(psurface,&pDesc->rendersurface,x,y,PX_ALIGN_LEFTTOP,PX_NULL);
	

}

PX_OBJECT_FREE_FUNCTION(PX_Object_PanelFree)
{
	PX_Object_Panel* pDesc = PX_ObjectGetDesc(PX_Object_Panel, pObject);
	PX_TextureFree(&pDesc->rendersurface);
}


PX_Object* PX_Object_PanelAttachObject( PX_Object* pObject,px_int attachIndex)
{
	px_memorypool* mp=pObject->mp;

	PX_Object_Panel* pdesc;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_Panel*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_PANEL, 0, PX_Object_PanelRender, PX_Object_PanelFree,0, sizeof(PX_Object_Panel));
	PX_ASSERTIF(pdesc == PX_NULL);

	PX_ASSERTIF(pObject->Width < 0 || pObject->Height < 0);
	
	if (!PX_TextureCreate(pObject->mp, &pdesc->rendersurface, (px_int)pObject->Width, (px_int)pObject->Height))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	
	return pObject;
}

PX_Object * PX_Object_PanelCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,px_int height)
{
	PX_Object *pObject;

	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0);
	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}

	if(!PX_Object_PanelAttachObject(pObject,0))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	
	return pObject;
}

px_surface* PX_Object_PanelGetSurface(PX_Object* pObject)
{
	PX_Object_Panel* pDesc= (PX_Object_Panel*)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_PANEL);
	PX_ASSERTIF(pDesc==PX_NULL);
	return &pDesc->rendersurface;
}
