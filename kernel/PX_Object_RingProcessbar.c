#include "PX_Object_RingProcessbar.h"


PX_OBJECT_RENDER_FUNCTION(PX_Object_RingProcessBarRender)
{
	px_int x,y,r;
	px_char content[8]={0};
	px_float process;
	px_float inheritX,inheritY;

	PX_Object_RingProcessBar *pDesc=PX_ObjectGetDesc(PX_Object_RingProcessBar,pObject);
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	r=(px_int)pObject->diameter/2;
	
	process=(pDesc->Value-pDesc->Min)*1.0f/(pDesc->Max-pDesc->Min);

	if (process<0)
	{
		process=0;
	}
	if (process>1)
	{
		process=1;
	}

	PX_sprintf1(content,sizeof(content),"%1.2%",PX_STRINGFORMAT_FLOAT(process*100));
	PX_GeoDrawCircle(psurface,x,y,r, pDesc->linewidth+6,pDesc->BackgroundColor);
	PX_GeoDrawRing(psurface,x,y,(px_float)r, (px_float)pDesc->linewidth,pDesc->Color,-90,-90+(px_int)(360*process));
	PX_FontModuleDrawText(psurface,pDesc->fm,x,y,PX_ALIGN_CENTER,content,pDesc->Color);

}

PX_Object* PX_Object_RingProcessBarAttachObject( PX_Object* pObject,px_int attachIndex, px_int x, px_int y, px_int r, PX_FontModule* fm)
{
	px_memorypool* mp= pObject->mp;
	PX_Object_RingProcessBar Desc,*pdesc;
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

PX_Object * PX_Object_RingProcessBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int r,PX_FontModule *fm)
{
	PX_Object *pObject;
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)0,(px_float)0,0);
	if (pObject ==PX_NULL)
	{
		return PX_NULL;
	}
	if (!PX_Object_RingProcessBarAttachObject(pObject,0,x,y,r,fm))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}

px_void PX_Object_RingProcessBarSetColor( PX_Object *pRingProcessBar,px_color Color)
{
	PX_Object_RingProcessBar *pProcess=PX_Object_GetRingProcessBar(pRingProcessBar);
	pProcess->Color=Color;
}

px_void PX_Object_RingProcessBarSetBackgroundColor(PX_Object *pRingProcessBar,px_color Color)
{
	PX_Object_RingProcessBar *pProcess=PX_Object_GetRingProcessBar(pRingProcessBar);
	pProcess->BackgroundColor=Color;
}

px_void PX_Object_RingProcessBarSetValue( PX_Object *pRingProcessBar,px_int Value )
{
	PX_Object_RingProcessBar *pProcess=PX_Object_GetRingProcessBar(pRingProcessBar);
	pProcess->Value=Value;
}


px_void PX_Object_RingProcessBarSetMax( PX_Object *pRingProcessBar,px_int Max )
{
	PX_Object_RingProcessBar *pProcess=PX_Object_GetRingProcessBar(pRingProcessBar);
	pProcess->Max=Max;
}

px_int PX_Object_RingProcessBarGetMax(PX_Object* pRingProcessBar)
{
	PX_Object_RingProcessBar* pProcess = PX_Object_GetRingProcessBar(pRingProcessBar);
	return pProcess->Max;
}

PX_Object_RingProcessBar * PX_Object_GetRingProcessBar( PX_Object *pObject )
{
	PX_Object_RingProcessBar *pdesc=(PX_Object_RingProcessBar *)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_RINGPROGRESS);	
	PX_ASSERTIF(pdesc==PX_NULL);
	return pdesc;
}

px_int PX_Object_RingProcessBarGetValue( PX_Object *pRingProcessBar )
{
	return PX_Object_GetRingProcessBar(pRingProcessBar)->Value;
}

