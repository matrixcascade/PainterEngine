#include "PX_Object_Waiting.h"

PX_OBJECT_RENDER_FUNCTION(PX_Object_WaitingRender)
{
	px_rect rect = PX_ObjectGetRect(pObject);
	PX_Object_Waiting* pDesc = PX_ObjectGetDesc(PX_Object_Waiting, pObject);
	px_float x, y;
	
	pDesc->elapsed += elapsed*pDesc->speed;
	x = pObject->diameter / 2 * PX_cos_angle(pDesc->elapsed);
	y = pObject->diameter / 2 * PX_sin_angle(pDesc->elapsed);
	
	if (pDesc->lastx != rect.x || pDesc->lasty != rect.y)
	{
		pDesc->lastx = rect.x;
		pDesc->lasty = rect.y;
		pDesc->cursor->x = rect.x + x;
		pDesc->cursor->y = rect.y + y;
		PX_Object_CursorSliderReset(pDesc->cursor);
	}
	else
	{
		PX_Object_CursorSliderSetXY(pDesc->cursor, rect.x + x, rect.y + y);
	}

	PX_FontModuleDrawText(psurface, pDesc->fm, (px_int)(rect.x ),(px_int)(rect.y), PX_ALIGN_CENTER, pDesc->text.buffer, pDesc->color);
}

PX_OBJECT_FREE_FUNCTION(PX_Object_WaitingFree)
{
	PX_Object_Waiting* pDesc = PX_ObjectGetDesc(PX_Object_Waiting, pObject);
	PX_StringFree(&pDesc->text);
}


PX_Object* PX_Object_WaitingCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y,px_float r, px_float round_radius, px_float speed, px_color Color)
{
	PX_Object_Waiting* pDesc;
	PX_Object* pObject = PX_ObjectCreateEx(mp, Parent, x, y, 0, r * 2, r * 2, 0, PX_OBJECT_TYPE_WAITING, 0, PX_Object_WaitingRender, PX_Object_WaitingFree, 0, sizeof(PX_Object_Waiting));
	pDesc = PX_ObjectGetDescIndex(PX_Object_Waiting, pObject,0);
	pDesc->cursor = PX_Object_CursorSliderCreate(mp, Parent, x, y, round_radius, Color);
	PX_Object_CursorSliderDisableCursorTracking(pDesc->cursor, PX_TRUE);
	pDesc->elapsed = 0;
	pDesc->speed = speed;
	pDesc->color = Color;
	pDesc->lastx = x;
	pDesc->lasty = y;
	if (!PX_StringInitialize(mp, &pDesc->text))
	{
		return PX_NULL;
	}
	pObject->diameter = r * 2;
	return pObject;
}

px_void PX_Object_WaitingSetText(PX_Object* pObject, const px_char text[], PX_FontModule* fm)
{
	PX_Object_Waiting* pDesc = PX_ObjectGetDescIndex(PX_Object_Waiting, pObject, 0);
	pDesc->fm = fm;
	PX_StringSet(&pDesc->text, text);
}
