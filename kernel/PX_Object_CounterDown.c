#include "PX_Object_CounterDown.h"


px_void PX_Object_CounterDownUpdate(PX_Object* pObject, px_dword elapsed)
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
		}
	}
}


px_void PX_Object_CounterDownRender(px_surface* psurface, PX_Object* pObject, px_dword elapsed)
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

PX_Object* PX_Object_CounterDownCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius, PX_FontModule* fm)
{
	PX_Object_CounterDown Desc;
	PX_Object* pObject;
	Desc.ms_down = 0;
	Desc.ring_ms = 0;
	Desc.numericFM = fm;
	Desc.fontcolor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	Desc.backgroundcolor = PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	Desc.ringcolor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pObject= PX_ObjectCreateEx(mp, Parent, x, y, 0, radius * 2, radius * 2,0, PX_OBJECT_TYPE_COUNTERDOWN, PX_Object_CounterDownUpdate, PX_Object_CounterDownRender, 0, &Desc, sizeof(Desc));
	pObject->diameter = radius * 2;
	return pObject;
}

PX_Object_CounterDown* PX_Object_GetCounterDown(PX_Object* pObject)
{
	if (pObject->Type==PX_OBJECT_TYPE_COUNTERDOWN)
	{
		return PX_ObjectGetDesc(PX_Object_CounterDown, pObject);
	}
	return PX_NULL;
}

px_void PX_Object_CounterDownSetValue(PX_Object* pObject, px_int value)
{
	PX_Object_CounterDown* pdesc = PX_Object_GetCounterDown(pObject);
	if (pdesc)
	{
		pdesc->ms_down = value;
	}
}

px_int PX_Object_CounterDownGetValue(PX_Object* pObject)
{
	PX_Object_CounterDown* pdesc = PX_Object_GetCounterDown(pObject);
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
	if (pdesc)
	{
		pdesc->fontcolor = clr;
	}
}

px_void PX_Object_CounterDownSetBackgroundColor(PX_Object* pObject, px_color clr)
{
	PX_Object_CounterDown* pdesc = PX_Object_GetCounterDown(pObject);
	if (pdesc)
	{
		pdesc->backgroundcolor = clr;
	}
}


px_void PX_Object_CounterDownSetRingColor(PX_Object* pObject, px_color clr)
{
	PX_Object_CounterDown* pdesc = PX_Object_GetCounterDown(pObject);
	if (pdesc)
	{
		pdesc->ringcolor = clr;
	}
}