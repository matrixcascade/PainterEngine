#include "PX_Object_CounterDown.h"


PX_OBJECT_UPDATE_FUNCTION(PX_Object_CounterDownUpdate)
{
	PX_Object_CounterDown* pdesc = PX_ObjectGetDesc(PX_Object_CounterDown, pObject);
	if (pdesc->ms_down>(px_int)elapsed)
	{
		pdesc->ms_down -= elapsed;
	}
	else
	{
		if (pdesc->ms_down)
		{
			pdesc->ms_down = 0;
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_EXECUTE));
			pObject->Visible = PX_FALSE;
		}
	}
}


PX_OBJECT_RENDER_FUNCTION(PX_Object_CounterDownRender)
{
	px_char n[16];
	px_float objx, objy, objHeight, objWidth;
	px_float inheritX, inheritY;
	PX_Object_CounterDown* pdesc = PX_ObjectGetDesc(PX_Object_CounterDown, pObject);
	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);
	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;
	
	do 
	{
		px_float a;
		px_color clr;
		clr = pdesc->fontcolor;
		
		a = 1 - (pdesc->ms_down % 1000) / 1000.f;
		clr._argb.a = (px_byte)((1-a) * 255);
		PX_GeoDrawCircle(psurface, (px_int)objx, (px_int)objy, (px_int)( pObject->diameter / 2 + (pObject->diameter * (a))), (px_int)(16 * a), clr);

		if (pdesc->ms_down>500)
		{
			a = 1;
		}
		else
		{
			a = pdesc->ms_down / 500.f;
		}
		PX_GeoDrawSolidCircle(psurface, (px_int)objx, (px_int)objy, (px_int)(a*pObject->diameter / 2), pdesc->backgroundcolor);
		PX_GeoDrawSector(psurface, (px_int)objx, (px_int)objy, (a*(pObject->diameter / 2 - 5)), (a * (pObject->diameter / 2 - 1)), pdesc->ringcolor, -90, (px_int)(-90 + pdesc->ms_down % 1000 / 1000.f * 360));
		PX_itoa(pdesc->ms_down / 1000 + 1, n, 16, 10);
		PX_FontModuleDrawText(psurface, pdesc->numericFM, (px_int)objx, (px_int)objy, PX_ALIGN_CENTER, n, pdesc->fontcolor);

		
	} while (0);
	
}

PX_Object* PX_Object_CounterDownAttachObject(PX_Object* pObject,px_int attachIndex, px_float x, px_float y, px_float radius, PX_FontModule* fm)
{
	PX_Object_CounterDown *pdesc;
	px_memorypool* mp=pObject->mp;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_CounterDown*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_COUNTERDOWN, PX_Object_CounterDownUpdate, PX_Object_CounterDownRender, 0, 0, sizeof(PX_Object_CounterDown));
	PX_ASSERTIF(pdesc == PX_NULL);

	pdesc->ms_down = 0;
	pdesc->ring_ms = 0;
	pdesc->numericFM = fm;
	pdesc->fontcolor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pdesc->backgroundcolor = PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	pdesc->ringcolor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	return pObject;
}

PX_Object* PX_Object_CounterDownCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius, PX_FontModule* fm)
{
	PX_Object* pObject;
	pObject= PX_ObjectCreate(mp, Parent, x, y, 0, radius * 2, radius * 2,0);
	pObject->diameter = radius * 2;
	pObject->Visible = PX_FALSE;
	if (!pObject)
	{
		return PX_NULL;
	}
	if (!PX_Object_CounterDownAttachObject(pObject, 0, x, y, radius, fm))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}

PX_Object_CounterDown* PX_Object_GetCounterDown(PX_Object* pObject)
{
	PX_Object_CounterDown* pdesc;
	pdesc = (PX_Object_CounterDown*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_COUNTERDOWN);
	PX_ASSERTIF(!pdesc);
	return pdesc;
}

px_void PX_Object_CounterDownSetValue(PX_Object* pObject, px_int value)
{
	PX_Object_CounterDown* pdesc = PX_Object_GetCounterDown(pObject);
	PX_ASSERTIF(!pdesc);
	if (pdesc)
	{
		pdesc->ms_down = value;
		pObject->Visible = PX_TRUE;
	}
}

px_int PX_Object_CounterDownGetValue(PX_Object* pObject)
{
	PX_Object_CounterDown* pdesc = PX_Object_GetCounterDown(pObject);
	PX_ASSERTIF(!pdesc);
	if (pdesc)
	{
		return pdesc->ms_down;
	}
	PX_ASSERT();
	return 0;
}

px_void PX_Object_CounterDownSetFontColor(PX_Object* pObject, px_color clr)
{
	PX_Object_CounterDown* pdesc = PX_Object_GetCounterDown(pObject);
	PX_ASSERTIF(!pdesc);
	if (pdesc)
	{
		pdesc->fontcolor = clr;
	}
}

px_void PX_Object_CounterDownSetBackgroundColor(PX_Object* pObject, px_color clr)
{
	PX_Object_CounterDown* pdesc = PX_Object_GetCounterDown(pObject);
	PX_ASSERTIF(!pdesc);
	if (pdesc)
	{
		pdesc->backgroundcolor = clr;
	}
}


px_void PX_Object_CounterDownSetRingColor(PX_Object* pObject, px_color clr)
{
	PX_Object_CounterDown* pdesc = PX_Object_GetCounterDown(pObject);
	PX_ASSERTIF(!pdesc);
	if (pdesc)
	{
		pdesc->ringcolor = clr;
	}
}