#include "PX_Object_Panel.h"

px_void PX_Object_PanelRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
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

px_void PX_Object_PanelFree( PX_Object *pObject )
{
	PX_Object_Panel* pDesc = PX_ObjectGetDesc(PX_Object_Panel, pObject);
	PX_TextureFree(&pDesc->rendersurface);
}

PX_Object * PX_Object_PanelCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_Object_Panel *pDesc;

	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0,PX_OBJECT_TYPE_PANEL,PX_NULL,PX_Object_PanelRender,PX_Object_PanelFree,0,sizeof(PX_Object_Panel));
	if (pObject==PX_NULL)
	{
		return PX_NULL;
	}
	pDesc=PX_ObjectGetDesc(PX_Object_Panel,pObject);
	if (width > 0 && height > 0)
	{
		if (!PX_TextureCreate(pObject->mp, &pDesc->rendersurface, width, height))
		{
			PX_ObjectDelete(pObject);
			return PX_NULL;
		}
	}
	return pObject;
}

px_surface* PX_Object_PanelGetSurface(PX_Object* pObject)
{
	PX_Object_Panel* pDesc = PX_ObjectGetDesc(PX_Object_Panel, pObject);
	if (pObject->Type==PX_OBJECT_TYPE_PANEL)
	{
		return &pDesc->rendersurface;
	}
	return PX_NULL;
}
