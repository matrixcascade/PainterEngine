#include "PX_Object_Processbar.h"


px_void PX_Object_ProcessBarRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	px_int px,py,pl;//Processing start X,Processing Start Y &Processing length
	px_int x,y,w,h;
	px_float inheritX,inheritY;

	PX_Object_ProcessBar *pProcessBar=(PX_Object_ProcessBar *)pObject->pObjectDesc;
	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	x=(px_int)(pObject->x+inheritX);
	y=(px_int)(pObject->y+inheritY);
	w=(px_int)pObject->Width;
	h=(px_int)pObject->Height;



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

PX_Object * PX_Object_ProcessBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height )
{
	PX_Object *pObject;
	PX_Object_ProcessBar *ProcessBar=(PX_Object_ProcessBar *)MP_Malloc(mp,sizeof(PX_Object_ProcessBar));
	if (ProcessBar==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,ProcessBar);
		return PX_NULL;
	}

	pObject->pObjectDesc=ProcessBar;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_PROCESSBAR;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=PX_NULL;
	pObject->Func_ObjectRender=PX_Object_ProcessBarRender;

	ProcessBar->MAX=100;
	ProcessBar->Value=0;
	ProcessBar->Color=PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	ProcessBar->BackgroundColor= PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
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
	if(pObject->Type==PX_OBJECT_TYPE_PROCESSBAR)
		return (PX_Object_ProcessBar *)pObject->pObjectDesc;
	else
		return PX_NULL;
}

px_int PX_Object_ProcessBarGetValue( PX_Object *pProcessBar )
{
	if (pProcessBar->Type!=PX_OBJECT_TYPE_PROCESSBAR)
	{
		PX_ASSERT();
		return 0;
	}
	return PX_Object_GetProcessBar(pProcessBar)->Value;
}

//////////////////////////////////////////////////////////////////////////
//process bar
//////////////////////////////////////////////////////////////////////////
PX_Object* PX_Designer_ProcessBarCreate(px_memorypool* mp, PX_Object* pparent, px_float x, px_float y, px_float width, px_float height, px_void* ptr)
{
	PX_FontModule* fm = (PX_FontModule*)ptr;
	return PX_Object_ProcessBarCreate(mp, pparent, (px_int)x, (px_int)y, 192, 32);
}

px_void PX_Designer_ProcessBarSetMax(PX_Object* pobject, px_int max)
{
	PX_Object_ProcessBarSetMax(pobject, max);
}

px_int PX_Designer_ProcessBarGetMax(PX_Object* pobject)
{
	return PX_Object_ProcessBarGetMax(pobject);
}

px_void PX_Designer_ProcessBarSetValue(PX_Object* pobject, px_int value)
{
	PX_Object_ProcessBarSetValue(pobject, value);
}

px_int PX_Designer_ProcessBarGetValue(PX_Object* pobject)
{
	return PX_Object_ProcessBarGetValue(pobject);
}

PX_Designer_ObjectDesc PX_Object_ProcessBarDesignerInstall()
{
	PX_Designer_ObjectDesc processbar;
	px_int i = 0;
	PX_memset(&processbar, 0, sizeof(processbar));
	PX_strcat(processbar.Name, "processbar");

	processbar.createfunc = PX_Designer_ProcessBarCreate;
	processbar.type = PX_DESIGNER_OBJECT_TYPE_UI;

	PX_strcat(processbar.properties[i].Name, "id");
	processbar.properties[i].getstring = PX_Designer_GetID;
	processbar.properties[i].setstring = PX_Designer_SetID;
	i++;

	PX_strcat(processbar.properties[i].Name, "x");
	processbar.properties[i].getfloat = PX_Designer_GetX;
	processbar.properties[i].setfloat = PX_Designer_SetX;
	i++;

	PX_strcat(processbar.properties[i].Name, "y");
	processbar.properties[i].getfloat = PX_Designer_GetY;
	processbar.properties[i].setfloat = PX_Designer_SetY;
	i++;

	PX_strcat(processbar.properties[i].Name, "width");
	processbar.properties[i].getfloat = PX_Designer_GetWidth;
	processbar.properties[i].setfloat = PX_Designer_SetWidth;
	i++;

	PX_strcat(processbar.properties[i].Name, "height");
	processbar.properties[i].getfloat = PX_Designer_GetHeight;
	processbar.properties[i].setfloat = PX_Designer_SetHeight;
	i++;

	PX_strcat(processbar.properties[i].Name, "max");
	processbar.properties[i].setint = PX_Designer_ProcessBarSetMax;
	processbar.properties[i].getint = PX_Designer_ProcessBarGetMax;
	i++;

	PX_strcat(processbar.properties[i].Name, "value");
	processbar.properties[i].setint = PX_Designer_ProcessBarSetValue;
	processbar.properties[i].getint = PX_Designer_ProcessBarGetValue;
	i++;

	return processbar;

}

