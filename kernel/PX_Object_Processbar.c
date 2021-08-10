#include "PX_Object_Processbar.h"


px_void PX_Object_ProcessBarRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	px_int px,py,pl;//Processing start X,Processing Start Y &Processing length
	px_int x,y,w,h;
	px_float inheritX,inheritY;

	PX_Object_ProcessBar *pProcessBar=(PX_Object_ProcessBar *)pObject->pObject;
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

	pObject->pObject=ProcessBar;
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

PX_Object_ProcessBar * PX_Object_GetProcessBar( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_PROCESSBAR)
		return (PX_Object_ProcessBar *)Object->pObject;
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

