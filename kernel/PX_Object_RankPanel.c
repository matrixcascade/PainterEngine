#include "PX_Object_RankPanel.h"

px_void PX_Object_RankPanelRender(px_surface* psurface, PX_Object* pObject, px_uint elapsed)
{
	PX_Object_RankPanel* pDesc = PX_ObjectGetDesc(PX_Object_RankPanel, pObject);
	px_float currentStage=pDesc->current_value/pDesc->max*pDesc->stage;
	px_int angle,colorIndex;
	px_float objx, objy, objHeight, objWidth;
	px_float inheritX, inheritY;
	px_color aColor;

	PX_ObjectGetInheritXY(pObject, &inheritX, &inheritY);

	objx = (pObject->x + inheritX);
	objy = (pObject->y + inheritY);
	objWidth = pObject->Width;
	objHeight = pObject->Height;

	
	pDesc->a1_angle += (PX_OBJECT_RANKPANEL_RING1_NORMAL_SPEED+(PX_OBJECT_RANKPANEL_RING1_MAX_SPEED- PX_OBJECT_RANKPANEL_RING1_NORMAL_SPEED)*(pDesc->current_value)/ pDesc->max) *elapsed / 1000;
	if (pDesc->a1_angle > 360) { pDesc->a1_angle -= (px_int)(pDesc->a1_angle / 360) * 360.f; }
	pDesc->a2_angle += (PX_OBJECT_RANKPANEL_RING2_NORMAL_SPEED+(PX_OBJECT_RANKPANEL_RING2_MAX_SPEED - PX_OBJECT_RANKPANEL_RING2_NORMAL_SPEED) * ( pDesc->current_value)/ pDesc->max) * elapsed / 1000;
	if (pDesc->a2_angle > 360) { pDesc->a2_angle -= (px_int)(pDesc->a2_angle / 360) * 360.f; }

	if (pDesc->current_value < pDesc->target_value)
	{
		px_float inc = pDesc->speed * elapsed / 1000;
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
		px_float inc = pDesc->speed * elapsed / 1000;
		if (inc > pDesc->current_value - pDesc->target_value)
		{
			pDesc->current_value = pDesc->target_value;
		}
		else
		{
			pDesc->current_value -= inc;
		}
	}

	


	if (currentStage>pDesc->stage)
	{
		currentStage = (px_float)pDesc->stage;
	}
	if (currentStage>1)
	{
		colorIndex = (px_int)currentStage-1;
	
		PX_GeoDrawSolidCircle(psurface, (px_int)objx, (px_int)objy, (px_int)pObject->diameter / 2, pDesc->rankColor[colorIndex]);
		
	}
	else
	{
		
		PX_GeoDrawSolidCircle(psurface, (px_int)objx, (px_int)objy, (px_int)pObject->diameter / 2, pDesc->backgroundColor);

	}
	angle = (px_int)(PX_FRAC(currentStage) * 360);
	colorIndex = (px_int)currentStage;

	aColor = pDesc->rankColor[colorIndex];
	
	PX_GeoDrawSector(psurface, (px_int)objx, (px_int)objy, pObject->diameter / 2, 0, pDesc->rankColor[colorIndex], -90, -90 + angle);

	PX_GeoDrawSector(psurface, (px_int)objx, (px_int)objy, pObject->diameter / 2 + 15, pObject->diameter / 2 + 5, aColor, (px_int)pDesc->a1_angle, (px_int)pDesc->a1_angle + 90);
	PX_GeoDrawSector(psurface, (px_int)objx, (px_int)objy, pObject->diameter / 2 + 15, pObject->diameter / 2 + 5, aColor, (px_int)pDesc->a1_angle+180, (px_int)pDesc->a1_angle + 270);

	PX_GeoDrawSector(psurface, (px_int)objx, (px_int)objy, pObject->diameter / 2 + 27, pObject->diameter / 2 + 39, aColor, (px_int)pDesc->a2_angle, (px_int)pDesc->a2_angle + 45);
	PX_GeoDrawSector(psurface, (px_int)objx, (px_int)objy, pObject->diameter / 2 + 27, pObject->diameter / 2 + 39, aColor, (px_int)pDesc->a2_angle + 90, (px_int)pDesc->a2_angle + 135);
	PX_GeoDrawSector(psurface, (px_int)objx, (px_int)objy, pObject->diameter / 2 + 27, pObject->diameter / 2 + 39, aColor, (px_int)pDesc->a2_angle+180, (px_int)pDesc->a2_angle + 225);
	PX_GeoDrawSector(psurface, (px_int)objx, (px_int)objy, pObject->diameter / 2 + 27, pObject->diameter / 2 + 39, aColor, (px_int)pDesc->a2_angle + 270, (px_int)pDesc->a2_angle + 315);

	do 
	{
		px_char content[32];
		PX_itoa((px_int)pDesc->current_value, content, sizeof(content), 10);
		PX_FontModuleDrawText(psurface, pDesc->numericFM, (px_int)objx, (px_int)objy, PX_ALIGN_CENTER, content, PX_ColorInverse(pDesc->rankColor[colorIndex]));
	} while (0);

}


PX_Object* PX_Object_RankPanelCreate(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float radius, PX_FontModule* fm)
{
	PX_Object_RankPanel Desc;
	PX_Object* pObject;
	PX_memset(&Desc, 0, sizeof(Desc));
	Desc.current_value = 0;
	Desc.max = 100;
	Desc.numericFM = fm;
	Desc.rankColor[0]=	PX_COLOR(255, 226, 228,240);
	Desc.rankColor[1] = PX_COLOR(255, 135, 192, 247);
	Desc.rankColor[2] = PX_COLOR(255, 192, 225, 118);
	Desc.rankColor[3] = PX_COLOR(255, 225, 100, 200);
	Desc.rankColor[4] = PX_COLOR(255, 235, 144, 152);
	Desc.rankColor[5] = PX_COLOR(255, 249, 239, 204);
	Desc.rankColor[6] = PX_COLOR(255, 237, 83, 71);
	Desc.rankColor[7] = PX_COLOR(255, 170, 95, 196);
	Desc.rankColor[8] = PX_COLOR(255, 112, 255, 182);
	Desc.speed = 100;
	Desc.stage = 8;
	Desc.target_value = 0;
	Desc.backgroundColor = PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pObject = PX_ObjectCreateEx(mp, Parent, x, y, 0, radius + radius, radius + radius, 0, PX_OBJECT_TYPE_RANKPANEL, PX_NULL, PX_Object_RankPanelRender, PX_NULL, &Desc, sizeof(Desc));
	pObject->diameter = radius * 2;
	return pObject;


}

PX_Object_RankPanel* PX_Object_GetRankPanel(PX_Object* pObject)
{
	if (pObject->Type==PX_OBJECT_TYPE_RANKPANEL)
	{
		return PX_ObjectGetDesc(PX_Object_RankPanel, pObject);
	}
	return PX_NULL;
}

px_void PX_Object_RankPanelSetValue(PX_Object* pObject, px_float value)
{
	PX_Object_RankPanel* pDesc = PX_Object_GetRankPanel(pObject);
	if (pDesc)
	{
		if (value<0)
		{
			value = 0;
		}
		if (value>pDesc->max)
		{
			value = pDesc->max;
		}
		pDesc->target_value = value;
	}
}


px_void PX_Object_RankPanelSetMaxValue(PX_Object* pObject, px_float value)
{
	PX_Object_RankPanel* pDesc = PX_Object_GetRankPanel(pObject);
	if (pDesc)
	{
		pDesc->max = value;
	}
}

px_void PX_Object_RankPanelSetStage(PX_Object* pObject, px_int stage)
{
	PX_Object_RankPanel* pDesc = PX_Object_GetRankPanel(pObject);
	if (pDesc)
	{
		if (stage>0&&stage<=PX_COUNTOF(pDesc->rankColor))
		{
			pDesc->stage = stage;
		}
	}
}

px_void PX_Object_RankPanelSetCurrentValue(PX_Object* pObject, px_float value)
{
	PX_Object_RankPanel* pDesc = PX_Object_GetRankPanel(pObject);
	if (pDesc)
	{
		pDesc->current_value = value;
	}
}

px_float PX_Object_RankPanelGetValue(PX_Object* pObject)
{
	PX_Object_RankPanel* pDesc = PX_Object_GetRankPanel(pObject);
	if (pDesc)
	{
		return pDesc->target_value;
	}
	return 0;
}

px_void PX_Object_RankPanelSetSpeed(PX_Object* pObject, px_float value)
{
	PX_Object_RankPanel* pDesc = PX_Object_GetRankPanel(pObject);
	if (pDesc)
	{
		pDesc->speed = value;
	}
}

