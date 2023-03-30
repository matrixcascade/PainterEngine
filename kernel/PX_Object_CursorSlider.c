#include "PX_Object_CursorSlider.h"

px_void PX_Object_CursorSliderUpdate(PX_Object* pObject, px_dword elapsed)
{
	PX_Object_CursorSlider	*pDesc= PX_ObjectGetDesc(PX_Object_CursorSlider, pObject);
	px_point2D v;
	px_float d, vd;


	vd = elapsed / 1000.0f * pDesc->reg_velocity;
	v = PX_Point2DSub(PX_POINT2D(pDesc->reg_x, pDesc->reg_y), PX_POINT2D(pObject->x,pObject->y));
	d = PX_Point2DMod(v);
	v = PX_Point2DNormalization(v);

	if (vd >= d)
	{
		pObject->x = pDesc->reg_x;
		pObject->y = pDesc->reg_y;
	}
	else
	{
		pObject->x += v.x * vd;
		pObject->y += v.y * vd;
	}
}

px_void PX_Object_CursorSliderRender(px_surface* psurface, PX_Object* pObject, px_dword elapsed)
{
	px_int i;
	px_float d;
	px_float a=0,target_a;
	px_dword update_elapsed=elapsed;
	px_point2D current_p,target_p,v;
	PX_Object_CursorSlider* pDesc = PX_ObjectGetDesc(PX_Object_CursorSlider, pObject);

	while (update_elapsed)
	{
		if(update_elapsed >= pDesc->atom_update_duration)
			update_elapsed -= pDesc->atom_update_duration;
		else
			update_elapsed = 0;

		pDesc->sliderpoints[0].x = pObject->x;
		pDesc->sliderpoints[0].y = pObject->y;
	
		for (i = 1; i < PX_COUNTOF(pDesc->sliderpoints); i++)
		{
			d = PX_Point2DDistance(pDesc->sliderpoints[i], pDesc->sliderpoints[i - 1]);
			if (d <= 0.1f)
			{
				pDesc->sliderpoints[i] = pDesc->sliderpoints[i - 1];
			}
			else
			{
				pDesc->sliderpoints[i].x = pDesc->sliderpoints[i].x + (pDesc->sliderpoints[i - 1].x - pDesc->sliderpoints[i].x) / 2;
				pDesc->sliderpoints[i].y = pDesc->sliderpoints[i].y + (pDesc->sliderpoints[i - 1].y - pDesc->sliderpoints[i].y) / 2;
			}
		}
	}

	current_p = pDesc->sliderpoints[PX_OBJECT_CURSORSLIDER_COUNT - 1];
	for (i= PX_COUNTOF(pDesc->sliderpoints)-2;i>=0;i--)
	{
		px_int k;
		px_float step_a;
		target_p = pDesc->sliderpoints[i];
		target_a = (PX_OBJECT_CURSORSLIDER_COUNT - i) *1.0f/ (PX_OBJECT_CURSORSLIDER_COUNT-1);
		v = PX_Point2DSub(target_p, current_p);
		d = PX_Point2DMod(v);
		if ((px_int)d==0)
		{
			break;
		}
		v = PX_Point2DNormalization(v);
		step_a = (target_a - a) / d;
		for (k=0;k<(px_int)d;k++)
		{
			a += step_a;
			current_p=PX_Point2DAdd(current_p, v);
			PX_GeoDrawPenCircle(psurface, current_p.x, current_p.y, pObject->diameter / 2 * a, pDesc->color);
		}
	}
	PX_GeoDrawPenCircle(psurface, pDesc->sliderpoints[0].x, pDesc->sliderpoints[0].y, pObject->diameter / 2, pDesc->color);
}

px_void PX_Object_CursorSliderSetXY(PX_Object* pObject, px_float x, px_float y)
{
	if (pObject->Type==PX_OBJECT_TYPE_CURSORSLIDER)
	{
		PX_Object_CursorSlider* pDesc = PX_ObjectGetDesc(PX_Object_CursorSlider, pObject);
		pDesc->reg_x = x;
		pDesc->reg_y = y;
		return;
	}
	PX_ASSERT();
}

px_void PX_Object_CursorSliderOnMove(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_CursorSliderSetXY(pObject, PX_Object_Event_GetCursorX(e), PX_Object_Event_GetCursorY(e));
}

PX_Object* PX_Object_CursorSliderCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius, px_color Color)
{
	PX_Object_CursorSlider desc = { 0 }, * pDesc;
	PX_Object* pObject;
	px_int i;
	pObject = PX_ObjectCreateEx(mp, Parent, x, y, 0, radius * 2, radius * 2, 0, PX_OBJECT_TYPE_CURSORSLIDER, PX_Object_CursorSliderUpdate, PX_Object_CursorSliderRender, 0, &desc, sizeof(desc));
	pDesc = PX_ObjectGetDesc(PX_Object_CursorSlider, pObject);
	pDesc->reg_x = x;
	pDesc->reg_y = y;
	pDesc->color = Color;
	pDesc->atom_update_duration = 30;
	pDesc->reg_velocity = 2000;
	pObject->diameter = radius * 2;
	for (i=0;i<PX_COUNTOF(pDesc->sliderpoints);i++)
	{
		pDesc->sliderpoints[i].x = x;
		pDesc->sliderpoints[i].y = y;
	}
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_CursorSliderOnMove, 0);
	return pObject;
}

