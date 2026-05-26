#include "PX_Object_Processbar.h"


PX_OBJECT_RENDER_FUNCTION(PX_Object_ProcessBarRender)
{
	px_int px,py,pl;//Processing start X,Processing Start Y &Processing length
	px_int x,y,w,h;
	px_rect rect;

	PX_Object_ProcessBar *pProcessBar=PX_ObjectGetDesc(PX_Object_ProcessBar,pObject);
	rect = PX_ObjectGetRect(pObject);
	x = (px_int)rect.x;
	y = (px_int)rect.y;
	w = (px_int)rect.width;
	h = (px_int)rect.height;



	px=(px_int)x+2; //increase line width
	py=(px_int)y+2;

	if (pProcessBar->Value<0)
	{
		pProcessBar->Value=0;
	}
	if (pProcessBar->Value>pProcessBar->MAX)
	{
		pProcessBar->Value=pProcessBar->MAX;
	}

	if (w<=4)
	{
		return;
	}
	if(pProcessBar->Value<pProcessBar->MAX)
		pl=(px_int)((w-4)*((pProcessBar->Value*1.0)/pProcessBar->MAX));
	else
		pl=(px_int)w-2;
	//Draw BackGround
	PX_SurfaceClear(
		psurface,
		(px_int)x,
		(px_int)y,
		(px_int)x+w-1,
		(px_int)y+h-1,
		pProcessBar->BackgroundColor
		);
	//Draw border
	PX_GeoDrawBorder(
		psurface,
		x,
		y,
		x+w-1,
		y+h-1,
		1,
		pProcessBar->Color
		);

	PX_GeoDrawRect(psurface,px,py,px+pl-2-1,py+(px_int)h-4-1,pProcessBar->Color);

	//Draw text
	if (pProcessBar->ProgressTextColor._argb.a)
	{
		px_char text[32];
		PX_sprintf1(text,sizeof(text),"%1%",PX_STRINGFORMAT_INT((px_int)((pProcessBar->Value*1.0/pProcessBar->MAX)*100)));
		PX_FontModuleDrawText(psurface, 0,x+w/2,y+h/2,PX_ALIGN_CENTER, text,pProcessBar->ProgressTextColor);
	}

}

PX_Object* PX_Object_ProcessBarAttachObject( PX_Object* pObject,px_int attachIndex, px_int x, px_int y, px_int Width, px_int Height)
{
	px_memorypool* mp= pObject->mp;

	PX_Object_ProcessBar* ProcessBar;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	ProcessBar = (PX_Object_ProcessBar*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_PROCESSBAR, 0, PX_Object_ProcessBarRender, 0, 0, sizeof(PX_Object_ProcessBar));
	PX_ASSERTIF(ProcessBar == PX_NULL);

	ProcessBar->MAX = 100;
	ProcessBar->Value = 0;
	ProcessBar->Color = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	ProcessBar->BackgroundColor = PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	return pObject;
}


PX_Object * PX_Object_ProcessBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height )
{
	PX_Object *pObject;
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);
	if (pObject ==PX_NULL)
	{
		return PX_NULL;
	}

	if(!PX_Object_ProcessBarAttachObject(pObject,0,0,0,Width,Height))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	
	return pObject;
}

px_void PX_Object_ProcessBarSetColor( PX_Object *pProcessBar,px_color Color)
{
	PX_Object_ProcessBar *pProcess=PX_Object_GetProcessBar(pProcessBar);
	if (pProcess!=PX_NULL)
	{

		pProcess->Color=Color;
	}
}

px_void PX_Object_ProcessBarSetTextColor(PX_Object* pProcessBar, px_color Color)
{
	PX_Object_ProcessBar* pProcess = PX_Object_GetProcessBar(pProcessBar);
	if (pProcess != PX_NULL)
	{
		pProcess->ProgressTextColor = Color;
	}
}

px_void PX_Object_ProcessBarSetBackgroundColor(PX_Object *pProcessBar,px_color Color)
{
	PX_Object_ProcessBar *pProcess=PX_Object_GetProcessBar(pProcessBar);
	if (pProcess!=PX_NULL)
	{
		pProcess->BackgroundColor=Color;
	}
}

px_void PX_Object_ProcessBarSetValue( PX_Object *pProcessBar,px_int Value )
{
	PX_Object_ProcessBar *pProcess=PX_Object_GetProcessBar(pProcessBar);
	if (pProcess!=PX_NULL)
	{
		pProcess->Value=Value;
	}
}


px_void PX_Object_ProcessBarSetMax( PX_Object *pProcessBar,px_int Max )
{
	PX_Object_ProcessBar *pProcess=PX_Object_GetProcessBar(pProcessBar);
	if (pProcess!=PX_NULL)
	{

		pProcess->MAX=Max;
	}
}

px_int PX_Object_ProcessBarGetMax(PX_Object* pProcessBar)
{
	PX_Object_ProcessBar* pProcess = PX_Object_GetProcessBar(pProcessBar);
	if (pProcess != PX_NULL)
	{

		return pProcess->MAX;
	}
	PX_ASSERT();
	return 0;
}

PX_Object_ProcessBar * PX_Object_GetProcessBar( PX_Object *pObject )
{
	PX_Object_ProcessBar* pDesc;
	pDesc=(PX_Object_ProcessBar *)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_PROCESSBAR);
	PX_ASSERTIF(pDesc==PX_NULL);
	return pDesc;
	
}

px_int PX_Object_ProcessBarGetValue( PX_Object *pProcessBar )
{
	return PX_Object_GetProcessBar(pProcessBar)->Value;
}


PX_OBJECT_RENDER_FUNCTION(PX_Object_RingProcessBarRender)
{
	px_int x, y, r;
	px_char content[8] = { 0 };
	px_float process;
	px_rect rect;

	PX_Object_RingProcessBar* pDesc = PX_ObjectGetDesc(PX_Object_RingProcessBar, pObject);
	rect = PX_ObjectGetRect(pObject);

	x = (px_int)(rect.x);
	y = (px_int)(rect.y);
	r = (px_int)pObject->diameter / 2;

	process = (pDesc->Value - pDesc->Min) * 1.0f / (pDesc->Max - pDesc->Min);

	if (process < 0)
	{
		process = 0;
	}
	if (process > 1)
	{
		process = 1;
	}

	PX_sprintf1(content, sizeof(content), "%1.2%", PX_STRINGFORMAT_FLOAT(process * 100));
	PX_GeoDrawCircle(psurface, x, y, r, pDesc->linewidth + 6, pDesc->BackgroundColor);
	PX_GeoDrawRing(psurface, x, y, (px_float)r, (px_float)pDesc->linewidth, pDesc->Color, -90, -90 + (px_int)(360 * process));
	PX_FontModuleDrawText(psurface, pDesc->fm, x, y, PX_ALIGN_CENTER, content, pDesc->Color);

}

PX_Object* PX_Object_RingProcessBarAttachObject(PX_Object* pObject, px_int attachIndex, px_int x, px_int y, px_int r, PX_FontModule* fm)
{
	px_memorypool* mp = pObject->mp;
	PX_Object_RingProcessBar Desc, * pdesc;
	Desc.BackgroundColor = PX_OBJECT_UI_DEFAULT_PUSHCOLOR;
	Desc.Color = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	Desc.linewidth = 12;
	Desc.Max = 100;
	Desc.Min = 0;
	Desc.Value = 32;
	Desc.fm = fm;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_RingProcessBar*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_RINGPROGRESS, 0, PX_Object_RingProcessBarRender, 0, &Desc, sizeof(PX_Object_RingProcessBar));
	PX_ASSERTIF(pdesc == PX_NULL);

	pObject->diameter = r * 2.0f;
	return pObject;
}

PX_Object* PX_Object_RingProcessBarCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int r, PX_FontModule* fm)
{
	PX_Object* pObject;
	pObject = PX_ObjectCreate(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)0, (px_float)0, 0);
	if (pObject == PX_NULL)
	{
		return PX_NULL;
	}
	if (!PX_Object_RingProcessBarAttachObject(pObject, 0, x, y, r, fm))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}

px_void PX_Object_RingProcessBarSetColor(PX_Object* pRingProcessBar, px_color Color)
{
	PX_Object_RingProcessBar* pProcess = PX_Object_GetRingProcessBar(pRingProcessBar);
	pProcess->Color = Color;
}

px_void PX_Object_RingProcessBarSetBackgroundColor(PX_Object* pRingProcessBar, px_color Color)
{
	PX_Object_RingProcessBar* pProcess = PX_Object_GetRingProcessBar(pRingProcessBar);
	pProcess->BackgroundColor = Color;
}

px_void PX_Object_RingProcessBarSetValue(PX_Object* pRingProcessBar, px_int Value)
{
	PX_Object_RingProcessBar* pProcess = PX_Object_GetRingProcessBar(pRingProcessBar);
	pProcess->Value = Value;
}


px_void PX_Object_RingProcessBarSetMax(PX_Object* pRingProcessBar, px_int Max)
{
	PX_Object_RingProcessBar* pProcess = PX_Object_GetRingProcessBar(pRingProcessBar);
	pProcess->Max = Max;
}

px_int PX_Object_RingProcessBarGetMax(PX_Object* pRingProcessBar)
{
	PX_Object_RingProcessBar* pProcess = PX_Object_GetRingProcessBar(pRingProcessBar);
	return pProcess->Max;
}

PX_Object_RingProcessBar* PX_Object_GetRingProcessBar(PX_Object* pObject)
{
	PX_Object_RingProcessBar* pdesc = (PX_Object_RingProcessBar*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_RINGPROGRESS);
	PX_ASSERTIF(pdesc == PX_NULL);
	return pdesc;
}

px_int PX_Object_RingProcessBarGetValue(PX_Object* pRingProcessBar)
{
	return PX_Object_GetRingProcessBar(pRingProcessBar)->Value;
}

PX_OBJECT_RENDER_FUNCTION(PX_Object_RingProgressRender)
{
	PX_Object_RingProgress* pDesc = PX_ObjectGetDesc(PX_Object_RingProgress, pObject);
	px_int angle;
	px_float objx, objy, objHeight, objWidth;
	px_rect rect;

	rect = PX_ObjectGetRect(pObject);
	objx = rect.x;
	objy = rect.y;
	objWidth = rect.width;
	objHeight = rect.height;


	if (pDesc->current_value < pDesc->target_value)
	{
		px_float inc = pDesc->Speed * elapsed / 1000;
		if (inc > pDesc->target_value - pDesc->current_value)
		{
			pDesc->current_value = pDesc->target_value;
		}
		else
		{
			pDesc->current_value += inc;
		}
	}
	else if (pDesc->current_value > pDesc->target_value)
	{
		px_float inc = pDesc->Speed * elapsed / 1000;
		if (inc > pDesc->current_value - pDesc->target_value)
		{
			pDesc->current_value = pDesc->target_value;
		}
		else
		{
			pDesc->current_value -= inc;
		}
	}



	PX_GeoDrawRing(psurface, (px_int)objx, (px_int)objy, pObject->diameter / 2, 6, pDesc->backgroundColor, 0, 360);
	PX_GeoDrawRing(psurface, (px_int)objx, (px_int)objy, pObject->diameter / 2, 7, pDesc->borderColor, 0, 360);


	angle = (px_int)((pDesc->current_value) / pDesc->max * 360);

	PX_GeoDrawRing(psurface, (px_int)objx, (px_int)objy, pObject->diameter / 2, 5, pDesc->processColor, -90, -90 + angle);


	do
	{
		px_char content[32];
		if (pDesc->target_value - (px_int)pDesc->target_value)
		{
			PX_sprintf1(content, sizeof(content), "%1.2", PX_STRINGFORMAT_FLOAT(pDesc->current_value));
			PX_FontModuleDrawText(psurface, pDesc->numericFM, (px_int)objx, (px_int)objy, PX_ALIGN_CENTER, content, (pDesc->fontColor));
		}
		else
		{
			PX_itoa((px_int)pDesc->current_value, content, sizeof(content), 10);
			PX_FontModuleDrawText(psurface, pDesc->numericFM, (px_int)objx, (px_int)objy, PX_ALIGN_CENTER, content, (pDesc->fontColor));
		}

	} while (0);

}

PX_Object* PX_Object_RingProgressAttachObject(PX_Object* pObject, px_int attachIndex, px_float x, px_float y, px_float radius, PX_FontModule* fm)
{
	PX_Object_RingProgress Desc, * pdesc;

	PX_memset(&Desc, 0, sizeof(Desc));
	Desc.current_value = 0;
	Desc.max = 100;
	Desc.numericFM = fm;
	Desc.backgroundColor = PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	Desc.borderColor = PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	Desc.processColor = PX_COLOR(128, 255, 64, 64);
	Desc.fontColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	Desc.min = 0;
	Desc.target_value = 0;
	Desc.Speed = 100;
	Desc.target_value = 0;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_RingProgress*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_RINGPROGRESS, 0, PX_Object_RingProgressRender, 0, &Desc, sizeof(Desc));
	PX_ASSERTIF(pdesc == PX_NULL);

	pObject->diameter = radius * 2;
	return pObject;
}


PX_Object* PX_Object_RingProgressCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius, PX_FontModule* fm)
{
	PX_Object* pObject = PX_ObjectCreate(mp, Parent, x, y, 0, radius + radius, radius + radius, 0);
	if (!pObject)
	{
		return PX_NULL;
	}
	if (!PX_Object_RingProgressAttachObject(pObject, 0, 0, 0, radius, fm))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}

	return pObject;
}

PX_Object_RingProgress* PX_Object_GetRingProgress(PX_Object* pObject)
{
	PX_Object_RingProgress* pdesc = (PX_Object_RingProgress*)PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_RINGPROGRESS);
	PX_ASSERTIF(pdesc == PX_NULL);
	return pdesc;

}

px_void PX_Object_RingProgressSetValue(PX_Object* pObject, px_float value)
{
	PX_Object_RingProgress* pDesc = PX_Object_GetRingProgress(pObject);
	if (pDesc)
	{
		if (value < 0)
		{
			value = 0;
		}
		if (value > pDesc->max)
		{
			value = pDesc->max;
		}
		pDesc->target_value = value;
	}
}


px_void PX_Object_RingProgressSetMaxValue(PX_Object* pObject, px_float value)
{
	PX_Object_RingProgress* pDesc = PX_Object_GetRingProgress(pObject);
	if (pDesc)
	{
		pDesc->max = value;

	}
}


px_void PX_Object_RingProgressSetCurrentValue(PX_Object* pObject, px_float value)
{
	PX_Object_RingProgress* pDesc = PX_Object_GetRingProgress(pObject);
	if (pDesc)
	{
		pDesc->current_value = value;
	}
}

px_float PX_Object_RingProgressGetValue(PX_Object* pObject)
{
	PX_Object_RingProgress* pDesc = PX_Object_GetRingProgress(pObject);
	if (pDesc)
	{
		return pDesc->target_value;
	}
	return 0;
}

px_void PX_Object_RingProgressSetSpeed(PX_Object* pObject, px_float value)
{
	PX_Object_RingProgress* pDesc = PX_Object_GetRingProgress(pObject);
	if (pDesc)
	{
		pDesc->Speed = value;
	}
}

