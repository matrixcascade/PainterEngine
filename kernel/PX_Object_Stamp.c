#include "PX_Object_Stamp.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_StampRender)
{
	px_float x, y;
	PX_Object_Stamp* pdesc = PX_ObjectGetDesc(PX_Object_Stamp, pObject);
	px_rect rect;
	rect = PX_ObjectGetRect(pObject);
	x = rect.x;
	y = rect.y;
	if (pdesc->elapsed<800)
	{
		px_float schedule = pdesc->elapsed / 800.f;
		px_float scale;
		PX_TEXTURERENDER_BLEND blend;

		blend.alpha = schedule;
		blend.hdr_R = 1;
		blend.hdr_G = 1;
		blend.hdr_B = 1;
		
		scale = 1+ (1-schedule * schedule) * 3;
		PX_TextureRenderEx(psurface, pdesc->ptexture, (px_int)(x), (px_int)(y), PX_ALIGN_CENTER, &blend, scale,0);
		pdesc->elapsed += elapsed;
	}
	else
	{
		PX_TextureRender(psurface, pdesc->ptexture, (px_int)(x), (px_int)(y), PX_ALIGN_CENTER, 0);
	}
}

PX_Object* PX_Object_StampCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y)
{
	PX_Object* pObject = PX_ObjectCreateEx(mp, Parent, x, y, 0, 0, 0, 0, PX_OBJECT_TYPE_STAMP, 0, PX_Object_StampRender, 0, 0, sizeof(PX_Object_Stamp));
	PX_Object_Stamp* pdesc = PX_ObjectGetDescIndex(PX_Object_Stamp, pObject,0);
	if (pObject == PX_NULL)
	{
		return PX_NULL;
	}
	PX_ObjectSetVisible(pObject, PX_FALSE);
	return pObject;
}

px_void PX_Object_StampPresent(PX_Object* pObject, px_texture* ptexture)
{
	PX_Object_Stamp* pdesc = PX_ObjectGetDescIndex(PX_Object_Stamp, pObject, 0);
	pdesc->ptexture = ptexture;
	pdesc->elapsed = 0;
	PX_ObjectSetVisible(pObject, PX_TRUE);
}