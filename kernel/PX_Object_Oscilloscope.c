#include "PX_Object_Oscilloscope.h"

//////////////////////////////////////////////////////////////////////////
//
static px_int PX_Object_OscilloscopeDichotomy(px_double *p,px_int Size,px_double Mark)
{
	px_int Minx,Maxx;
	px_int Index;
	if (Mark<=p[0])
	{
		return 0;
	}
	if (Mark>=p[Size-1])
	{
		return Size-1;
	}

	Minx=0;
	Maxx=Size-1;



	while (Maxx-Minx>1)
	{
		Index=(Maxx+Minx)/2;
		if (p[Index]>=Mark)
		{
			Maxx=Index;
		}
		else
		{
			Minx=Index;
		}
	}

	if (Index==Size-1)
	{
		return Minx;
	}


	if (Mark-p[Index]<p[Index+1]-Mark)
	{
		return Minx;
	}
	else
	{
		return Minx+1;
	}

}




PX_Object_Oscilloscope *PX_Object_GetOscilloscope(PX_Object *pObject)
{
	if (pObject->Type==PX_OBJECT_TYPE_OSCILLOSCOPE)
	{
		return (PX_Object_Oscilloscope *)pObject->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_OscilloscopeSetMinVerticalPixelDividing(PX_Object *pObj,px_int val)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->MinVerticalPixelDividing=val;
}

px_void PX_Object_OscilloscopeSetMinHorizontalPixelDividing(PX_Object *pObj,px_int val)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->MinHorizontalPixelDividing=val;
}

px_void PX_Object_OscilloscopeSetHorizontalDividing(PX_Object *pObj,px_int Count)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->HorizontalDividing=Count;
}

px_void PX_Object_OscilloscopeSetLeftVerticalDividing(PX_Object *pObj,px_int Count)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->LeftVerticalDividing=Count;
}

px_void PX_Object_OscilloscopeSetRightVerticalDividing(PX_Object *pObj,px_int Count)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->RightVerticalDividing=Count;
}



px_void PX_Object_OscilloscopeSetStyle(PX_Object *pObj,PX_OBJECT_OSCILLOSCOPE_LINEMODE mode)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObj);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->LineMode=mode;
}



px_void PX_Object_OscilloscopeSetScaleEnabled(PX_Object *pObject,px_bool Enabled)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->ScaleEnabled=Enabled;
}

px_void PX_Object_OscilloscopeSetGuidesVisible(PX_Object *pObject,px_bool Visible)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->ShowGuides=Visible;
}

px_void PX_Object_OscilloscopeSetGuidesShowMode(PX_Object *pObject,PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE mode)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->guidesShowMode=mode;
}


px_void PX_Object_OscilloscopeShowHelpLine(PX_Object *pObject,px_bool show)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->ShowHelpLine=show;
}

px_void PX_Object_OscilloscopeSetDataLineWidth(PX_Object *pObject,px_float linewidth)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->DataLineWidth=linewidth;
}

void PX_Object_OscilloscopeSetDataShow(PX_Object *pObject,px_int index,px_bool show)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	PX_Object_OscilloscopeData *pData;
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	if (index>=pcd->vData.size)
	{
		return;
	}
	pData=PX_VECTORAT(PX_Object_OscilloscopeData,&pcd->vData,index);
	pData->Visibled=show;
}

px_void PX_Object_OscilloscopeSetGuidesLineWidth(PX_Object *pObject,px_float linewidth)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->GuidesLineWidth=linewidth;
}

px_void PX_Object_OscilloscopeSetGuidesLineColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->helpLineColor=clr;
}

px_void PX_Object_OscilloscopeSetTitleFontSize(PX_Object *pObject,px_int size)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->FontSize=size;
}

px_void PX_Object_OscilloscopeSetTitleFontColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->FontColor=clr;
}

px_void PX_Object_OscilloscopeSetDashLineColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->DashColor=clr;
}

px_void PX_Object_OscilloscopeSetLeftTextShow(PX_Object *pObject,px_bool bshow)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->LeftTextShow=bshow;
}

px_void PX_Object_OscilloscopeSetRightTextShow(PX_Object *pObject,px_bool bshow)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->RightTextShow=bshow;
}

px_void PX_Object_OscilloscopeSetHorizontalTextShow(PX_Object *pObject,px_bool bshow)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->HorizontalTextShow=bshow;
}

px_void PX_Object_OscilloscopeSetFloatFlagFormatHorizontal(PX_Object *pObject,const px_char *fmt)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->FloatFlagFormat_H=fmt;
}

px_void PX_Object_OscilloscopeSetIntFlagFormatHorizontal(PX_Object *pObject,const px_char *fmt)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->IntFlagFormat_H=fmt;
}

px_void PX_Object_OscilloscopeSetFloatFlagFormatVerticalLeft(PX_Object *pObject,const px_char *fmt)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->FloatFlagFormat_L=fmt;
}

px_void PX_Object_OscilloscopeSetIntFlagFormatVerticalLeft(PX_Object *pObject,const px_char *fmt)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->IntFlagFormat_L=fmt;
}

px_void PX_Object_OscilloscopeSetFloatFlagFormatVerticalRight(PX_Object *pObject,const px_char *fmt)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->FloatFlagFormat_R=fmt;
}

px_void PX_Object_OscilloscopeSetIntFlagFormatVericalRight(PX_Object *pObject,const px_char *fmt)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->IntFlagFormat_R=fmt;
}

px_void PX_Object_OscilloscopeSetLeftTextMode(PX_Object *pObject,PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE mode)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->leftTextDisplayMode=mode;
}

px_void PX_Object_OscilloscopeSetRightTextMode(PX_Object *pObject,PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE mode)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->RightTextDisplayMode=mode;
}

px_void PX_Object_OscilloscopeSetHorizontalMin(PX_Object *pObject,px_double Min)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->HorizontalRangeMin=Min;
}

px_void PX_Object_OscilloscopeSetHorizontalMax(PX_Object *pObject,px_double Max)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->HorizontalRangeMax=Max;
}

px_void PX_Object_OscilloscopeSetLeftVerticalMin(PX_Object *pObject,px_double Min)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->LeftVerticalRangeMin=Min;
}

px_void PX_Object_OscilloscopeSetLeftVerticalMax(PX_Object *pObject,px_double Max)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->LeftVerticalRangeMax=Max;
}

px_void PX_Object_OscilloscopeSetRightVerticalMax(PX_Object *pObject,px_double Max)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->RightVerticalRangeMax=Max;
}

px_void PX_Object_OscilloscopeSetRightVerticalMin(PX_Object *pObject,px_double Min)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}
	pcd->RightVerticalRangeMin=Min;
}

px_void PX_Object_OscilloscopeSetBorderColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (pcd)
	{
		pcd->borderColor=clr;
	}
}

PX_Object_OscilloscopeData * PX_Object_OscilloscopeGetOscilloscopeData(PX_Object *pObject,px_int index)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (index>pcd->vData.size-1)
	{
		return PX_NULL;
	}
	return PX_VECTORAT(PX_Object_OscilloscopeData,&pcd->vData,index);
}

px_int PX_Object_OscilloscopeGetOscilloscopeWidth(PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);

	return (px_int)(pObject->Width-pcd->LeftSpacer-pcd->RightSpacer);
}

px_int PX_Object_OscilloscopeGetOscilloscopeHeight(PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);

	return (px_int)(pObject->Height-pcd->TopSpacer-pcd->BottomSpacer);
}

void PX_Object_OscilloscopeSetTitleTop(PX_Object *pObject,const px_char * title)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (pcd)
	{
		pcd->TopTitle=title;
	}
}

void PX_Object_OscilloscopeSetTitleBottom(PX_Object *pObject,const px_char * title)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (pcd)
	{
		pcd->BottomTitle=title;
	}
}

void PX_Object_OscilloscopeSetMarkValueEnabled(PX_Object *pObject,px_bool Enabled)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	pcd->MarkValueEnabled=Enabled;
}

void PX_Object_OscilloscopeSetFontColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (pcd)
	{
		pcd->FontColor=clr;
	}
}

void PX_Object_OscilloscopeClearContext(PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	PX_Object_OscilloscopeRestoreOscilloscope(pObject);
	PX_VectorClear(&pcd->vData);
}

void PX_Object_OscilloscopeClearFlagLine(PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	PX_VectorClear(&pcd->vFlagLine);
}

void PX_Object_OscilloscopeAddData(PX_Object *pObject,PX_Object_OscilloscopeData data)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	PX_VectorPushback(&pcd->vData,&data);
}

px_void PX_Object_OscilloscopeClearData(PX_Object* pObject)
{
	PX_Object_OscilloscopeClearContext(pObject);
}

void PX_Object_OscilloscopeAddOscilloscopeFlagLine(PX_Object *pObject,PX_Object_OscilloscopeFlagLine Line)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	PX_VectorPushback(&pcd->vFlagLine,&Line);
}


px_void PX_Object_OscilloscopeSetMargin(PX_Object *pObject,px_int Left,px_int Right,px_int Top,px_int Bottom)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	pcd->LeftSpacer=Left;
	pcd->RightSpacer=Right;
	pcd->TopSpacer=Top;
	pcd->BottomSpacer=Bottom;
}

px_int  PX_Object_OscilloscopeMapHorizontalValueToPixel(PX_Object *pObject,px_double val)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_double scale=PX_Object_OscilloscopeGetOscilloscopeWidth(pObject)/(pcd->HorizontalRangeMax-pcd->HorizontalRangeMin);
	return (px_int)((val-pcd->HorizontalRangeMin)*scale)+pcd->LeftSpacer;
}

px_int  PX_Object_OscilloscopeMapVerticalValueToPixel(PX_Object *pObject,px_double val,px_int Map)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_double scale;

	if(Map==PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT)
	{
		scale=PX_Object_OscilloscopeGetOscilloscopeHeight(pObject)/(pcd->LeftVerticalRangeMax-pcd->LeftVerticalRangeMin);
		return (px_int)(pObject->Height-pcd->BottomSpacer-(px_int)((val-pcd->LeftVerticalRangeMin)*scale));
	}
	else if(Map==PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT)
	{
		scale=PX_Object_OscilloscopeGetOscilloscopeHeight(pObject)/(pcd->RightVerticalRangeMax-pcd->RightVerticalRangeMin);
		return (px_int)(pObject->Height-pcd->BottomSpacer-(px_int)((val-pcd->RightVerticalRangeMin)*scale));
	}

	return 0;

}

px_double  PX_Object_OscilloscopeMapPixelValueToHorizontal(PX_Object *pObject,px_int Pixel)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_int XDisten=Pixel-pcd->LeftSpacer;
	return pcd->HorizontalRangeMin+XDisten*(pcd->HorizontalRangeMax-pcd->HorizontalRangeMin)/PX_Object_OscilloscopeGetOscilloscopeWidth(pObject);

}

px_double  PX_Object_OscilloscopeMapPixelValueToVertical(PX_Object *pObject,px_int Pixel,px_int Map)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_double Min,Max;
	if (Map==PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT)
	{
		Max=pcd->LeftVerticalRangeMax;
		Min=pcd->LeftVerticalRangeMin;
		return pcd->LeftVerticalRangeMin+(pObject->Height-pcd->BottomSpacer-Pixel)*(Max-Min)/PX_Object_OscilloscopeGetOscilloscopeHeight(pObject);
	}

	if (Map==PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT)
	{
		Max=pcd->RightVerticalRangeMax;
		Min=pcd->RightVerticalRangeMin;
		return pcd->RightVerticalRangeMin+(pObject->Height-pcd->BottomSpacer-Pixel)*(Max-Min)/PX_Object_OscilloscopeGetOscilloscopeHeight(pObject);
	}

	if (Map==PX_OBJECT_OSCILLOSCOPEDATA_MAP_HORIZONTAL)
	{
		Max=pcd->HorizontalRangeMax;
		Min=pcd->HorizontalRangeMin;
		return pcd->HorizontalRangeMin+(Pixel-pcd->LeftSpacer)*(Max-Min)/PX_Object_OscilloscopeGetOscilloscopeWidth(pObject);
	}
	return 0;
}


px_void PX_Object_OscilloscopeUpdate(PX_Object *pObject,px_uint elapsed)
{

}


static px_void PX_Object_OscilloscopeDrawFrameLine(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_int offsetx;
	px_int offsety;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	PX_GeoDrawLine(psurface,offsetx+pcd->LeftSpacer,offsety+pcd->TopSpacer,offsetx+(px_int)(objWidth-pcd->RightSpacer),offsety+pcd->TopSpacer,PX_OBJECT_OSCILLOSCOPE_DEFAULT_FRAMELINE_WIDTH,pcd->borderColor);
	PX_GeoDrawLine(psurface,offsetx+pcd->LeftSpacer,offsety+pcd->TopSpacer,offsetx+pcd->LeftSpacer,offsety+(px_int)(objHeight-pcd->BottomSpacer),PX_OBJECT_OSCILLOSCOPE_DEFAULT_FRAMELINE_WIDTH,pcd->borderColor);
	PX_GeoDrawLine(psurface,(px_int)(offsetx+objWidth-pcd->RightSpacer),offsety+(px_int)(objHeight-pcd->BottomSpacer),offsetx+(pcd->LeftSpacer),offsety+(px_int)objHeight-pcd->BottomSpacer,PX_OBJECT_OSCILLOSCOPE_DEFAULT_FRAMELINE_WIDTH,pcd->borderColor);
	PX_GeoDrawLine(psurface,(px_int)(offsetx+objWidth-pcd->RightSpacer),offsety+(px_int)(objHeight-pcd->BottomSpacer),offsetx+(px_int)(objWidth-pcd->RightSpacer),offsety+pcd->TopSpacer,PX_OBJECT_OSCILLOSCOPE_DEFAULT_FRAMELINE_WIDTH,pcd->borderColor);
}

static px_void PX_Object_OscilloscopeDrawDashed(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_double HorizontalInc=0;
	px_int i;
	px_double VerticalInc=0;
	px_int offsetx;
	px_int offsety;

	px_int Divid=PX_Object_OscilloscopeGetOscilloscopeWidth(pObject)/pcd->MinHorizontalPixelDividing;

	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;


	if (Divid>pcd->HorizontalDividing)
	{
		Divid=pcd->HorizontalDividing;
	}


	if(Divid>1)
	{
		HorizontalInc=(px_double)PX_Object_OscilloscopeGetOscilloscopeWidth(pObject)/Divid;
		if(pcd->guidesShowMode==PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE_ALL||pcd->guidesShowMode==PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE_HORIZONTAL)
		{
			for (i=1;i<=Divid;i++)
			{
				PX_GeoDrawLine(psurface,\
					offsetx+(px_int)(pcd->LeftSpacer+i*HorizontalInc),offsety+pcd->TopSpacer,\
					offsetx+(px_int)(pcd->LeftSpacer+i*HorizontalInc),offsety+pcd->TopSpacer+PX_Object_OscilloscopeGetOscilloscopeHeight(pObject),\
					1,\
					pcd->DashColor
					);
			}
		}
	}



	Divid=PX_Object_OscilloscopeGetOscilloscopeHeight(pObject)/pcd->MinVerticalPixelDividing;
	if (pcd->LeftVerticalDividing>=pcd->RightVerticalDividing)
	{
		if (Divid>pcd->LeftVerticalDividing)
		{
			Divid=pcd->LeftVerticalDividing;
		}
	}
	else
	{
		if (Divid>pcd->RightVerticalDividing)
		{
			Divid=pcd->RightVerticalDividing;
		}
	}

	if(Divid>1)
	{
		VerticalInc=(px_double)PX_Object_OscilloscopeGetOscilloscopeHeight(pObject)/Divid;
		if(pcd->guidesShowMode==PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE_ALL||pcd->guidesShowMode==PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE_VERTICAL)
		{	
			for (i=1;i<=Divid;i++)
			{
				PX_GeoDrawLine(psurface,\
					offsetx+(px_int)pcd->LeftSpacer,\
					offsety+(px_int)(objHeight-pcd->BottomSpacer-i*VerticalInc),\
					offsetx+(px_int)(pcd->LeftSpacer+PX_Object_OscilloscopeGetOscilloscopeWidth(pObject)),\
					offsety+(px_int)(objHeight-pcd->BottomSpacer-i*VerticalInc),\
					1,\
					pcd->DashColor\
					);
			}
		}
	}
}

static px_void PX_Object_OscilloscopeDrawDashText(px_surface *psurface,PX_Object *pObject)
{

	px_double HorizontalInc=0;
	px_double VerticalInc=0;
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_double ValInc;
	px_int	   IsFloat;
	px_int i;
	px_int offsetx;
	px_int offsety;

	px_int Divid=PX_Object_OscilloscopeGetOscilloscopeWidth(pObject)/pcd->MinHorizontalPixelDividing;

	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	if (Divid>pcd->HorizontalDividing)
	{
		Divid=pcd->HorizontalDividing;
	}



	if(Divid>0&&pcd->HorizontalTextShow)
	{
		HorizontalInc=(px_double)PX_Object_OscilloscopeGetOscilloscopeWidth(pObject)/Divid;
		ValInc=(pcd->HorizontalRangeMax-pcd->HorizontalRangeMin)/Divid;
		if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
		{
			IsFloat=PX_FALSE;
		}
		else
		{
			IsFloat=PX_TRUE;
		}

		//paint for horizontal coordinates text 
		for (i=0;i<=Divid;i++)
		{
			px_char text[32]={0};

			if(IsFloat)
				PX_sprintf1(text,sizeof(text),pcd->FloatFlagFormat_H,PX_STRINGFORMAT_FLOAT((px_float)(pcd->HorizontalRangeMin+i*ValInc)));
			else
				PX_sprintf1(text,sizeof(text),pcd->IntFlagFormat_H,PX_STRINGFORMAT_INT((px_int)(pcd->HorizontalRangeMin+i*ValInc)));

			PX_FontModuleDrawText(psurface,PX_NULL,offsetx+(px_int)(pcd->LeftSpacer+(i)*HorizontalInc),offsety+(px_int)(pObject->Height-pcd->BottomSpacer+PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLAGTEXT_SPACER+2),PX_ALIGN_CENTER,text,pcd->FontColor);
		}
	}

	Divid=PX_Object_OscilloscopeGetOscilloscopeHeight(pObject)/pcd->MinVerticalPixelDividing;
	if (pcd->LeftVerticalDividing>=pcd->RightVerticalDividing)
	{
		if (Divid>pcd->LeftVerticalDividing)
		{
			Divid=pcd->LeftVerticalDividing;
		}
	}
	else
	{
		if (Divid>pcd->RightVerticalDividing)
		{
			Divid=pcd->RightVerticalDividing;
		}
	}

	if(Divid>0)
	{
		VerticalInc=(px_double)PX_Object_OscilloscopeGetOscilloscopeHeight(pObject)/Divid;


		//paint for Left vertical coordinates text 
		if (pcd->LeftTextShow)
		{
			ValInc=(pcd->LeftVerticalRangeMax-pcd->LeftVerticalRangeMin)/Divid;
			if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
			{
				IsFloat=PX_FALSE;
			}
			else
			{
				IsFloat=PX_TRUE;
			}


			for (i=0;i<=Divid;i++)
			{
				px_char text[32]={0};

				switch (pcd->leftTextDisplayMode)
				{
				case PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE_EXPONENTRAL:
					{
						px_double value=PX_ABS(pcd->LeftVerticalRangeMin)>PX_ABS(pcd->LeftVerticalRangeMax)?PX_ABS(pcd->LeftVerticalRangeMin):PX_ABS(pcd->LeftVerticalRangeMax);
						px_int e=0;
						px_double e10=1;
						while(value>10){value/=10,e++;e10*=10;};
						value=pcd->LeftVerticalRangeMin+i*ValInc;
						value/=e10;

						if (e>=3)
						{
							PX_sprintf2(text,sizeof(text),pcd->Exponential_Format,PX_STRINGFORMAT_FLOAT((px_float)(value)),PX_STRINGFORMAT_INT(e));
						}
						else
						{
							if(IsFloat)
								PX_sprintf1(text,sizeof(text),pcd->FloatFlagFormat_L,PX_STRINGFORMAT_FLOAT((px_float)(pcd->LeftVerticalRangeMin+i*ValInc)));
							else
								PX_sprintf1(text,sizeof(text),pcd->IntFlagFormat_L,PX_STRINGFORMAT_INT((px_int)(pcd->LeftVerticalRangeMin+i*ValInc)));
						}


					}
					break;
				default:
				case PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE_NORMAL:
					{
						if(IsFloat)
							PX_sprintf1(text,sizeof(text),pcd->FloatFlagFormat_L,PX_STRINGFORMAT_FLOAT((px_float)(pcd->LeftVerticalRangeMin+i*ValInc)));
						else
							PX_sprintf1(text,sizeof(text),pcd->IntFlagFormat_L,PX_STRINGFORMAT_INT((px_int)(pcd->LeftVerticalRangeMin+i*ValInc)));
					}
					break;
				}

				PX_FontModuleDrawText(psurface,PX_NULL,offsetx+pcd->LeftSpacer-6,offsety+(px_int)(pObject->Height-pcd->BottomSpacer-(i)*VerticalInc-6),PX_ALIGN_RIGHTTOP,text,pcd->FontColor);
			}
		}


		//paint for Right vertical coordinates text
		if (pcd->RightTextShow)
		{
			ValInc=(pcd->RightVerticalRangeMax-pcd->RightVerticalRangeMin)/Divid;
			if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
			{
				IsFloat=PX_FALSE;
			}
			else
			{
				IsFloat=PX_TRUE;
			}


			for (i=0;i<=Divid;i++)
			{
				px_char text[32]={0};

				switch (pcd->leftTextDisplayMode)
				{
				case PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE_EXPONENTRAL:
					{
						px_double value=PX_ABS(pcd->RightVerticalRangeMin)>PX_ABS(pcd->RightVerticalRangeMax)?PX_ABS(pcd->RightVerticalRangeMin):PX_ABS(pcd->RightVerticalRangeMax);
						px_int e=0;
						px_double e10=1;
						while(value>10){value/=10,e++;e10*=10;};
						value=pcd->RightVerticalRangeMin+i*ValInc;
						value/=e10;

						if (e>=3)
						{
							PX_sprintf2(text,sizeof(text),pcd->Exponential_Format,PX_STRINGFORMAT_FLOAT((px_float)(value)),PX_STRINGFORMAT_INT(e));
						}
						else
						{
							if(IsFloat)
								PX_sprintf1(text,sizeof(text),pcd->FloatFlagFormat_R,PX_STRINGFORMAT_FLOAT((px_float)(pcd->LeftVerticalRangeMin+i*ValInc)));
							else
								PX_sprintf1(text,sizeof(text),pcd->IntFlagFormat_R,PX_STRINGFORMAT_INT((px_int)(pcd->LeftVerticalRangeMin+i*ValInc)));
						}


					}
					break;
				default:
				case PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE_NORMAL:
					{
						if(IsFloat)
							PX_sprintf1(text,sizeof(text),pcd->FloatFlagFormat_R,PX_STRINGFORMAT_FLOAT((px_float)(pcd->RightVerticalRangeMin+i*ValInc)));
						else
							PX_sprintf1(text,sizeof(text),pcd->IntFlagFormat_R,PX_STRINGFORMAT_INT((px_int)(pcd->RightVerticalRangeMin+i*ValInc)));
					}
					break;
				}
				PX_FontModuleDrawText(psurface,PX_NULL,offsetx+(px_int)(pObject->Width-pcd->RightSpacer+PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLAGTEXT_SPACER),offsety+(px_int)(pObject->Height-pcd->BottomSpacer-(i)*VerticalInc-6),PX_ALIGN_LEFTTOP,text,pcd->FontColor);
			}
		}
	}


}

static px_void PX_Object_OscilloscopeDrawTitle(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_int offsetx;
	px_int offsety;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;
	if (pcd->TopTitle[0])
	{
		PX_FontModuleDrawText(psurface,pcd->fontmodule,offsetx+(px_int)(pObject->Width/2),offsety,PX_ALIGN_MIDTOP,pcd->TopTitle,pcd->FontColor);
	}

	if (pcd->BottomTitle[0])
	{
		PX_FontModuleDrawText(psurface,pcd->fontmodule,offsetx+(px_int)(pObject->Width/2),offsety+(px_int)(pObject->Height-pcd->BottomSpacer+32),PX_ALIGN_MIDBOTTOM,pcd->BottomTitle,pcd->FontColor);
	}


	// 	px_int x = this->width()-20;
	// 
	// 	px_int y = ( this->height()/ 2);
	// 	painter.rotate(90);
	// 	painter.drawText(y-30, -x+12,m_RightTitle );
	// 
	// 	painter.rotate(180);
	// 	painter.drawText(-y-70, 45,m_LeftTitle);
}

static px_void PX_Object_OscilloscopeDrawDataInfo(px_surface *psurface,PX_Object *pObject,px_double *Horizontal,px_double n,px_double *Vertical,px_int linewidth,px_int Size,px_int Map,px_color Color)
{
	px_double RangeMin,RangeMax;
	px_double x,y,w,btm,zeroy;
	px_int dx1,dy1,dx2,dy2;
	px_int i;
	px_int offsetx;
	px_int offsety;

	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	if (Map==PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT)
	{
		RangeMin=pcd->LeftVerticalRangeMin;
		RangeMax=pcd->LeftVerticalRangeMax;
	}

	if (Map==PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT)
	{
		RangeMin=pcd->RightVerticalRangeMin;
		RangeMax=pcd->RightVerticalRangeMax;
	}

	switch(pcd->LineMode)
	{
	case PX_OBJECT_OSCILLOSCOPE_LINEMODE_PILLAR:
		{
			zeroy=PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,0,Map);
			for (i=0;i<Size;i++)
			{
				if (Horizontal[i]>=pcd->HorizontalRangeMin)
				{

					x=PX_Object_OscilloscopeMapHorizontalValueToPixel(pObject,Horizontal[i-1])-pcd->DataPillarWidth/2;
					y=PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,Vertical[i]/n,Map);

					w=pcd->DataPillarWidth;
					btm=zeroy;

					if (y>btm)
					{
						double tem;
						tem=btm;
						btm=y;
						y=tem;
					}

					if (!(btm<pcd->TopSpacer||y>pObject->Height-pcd->BottomSpacer))
					{
						if (y<pcd->TopSpacer)
						{
							y=pcd->TopSpacer;
						}
						if (btm>pObject->Height-pcd->BottomSpacer)
						{
							btm=pObject->Height-pcd->BottomSpacer;
						}
						if (x-(px_int)x>0.1)
						{
							PX_GeoDrawRect(psurface,offsetx+(px_int)x,offsety+(px_int)y,offsetx+(px_int)(x+w),offsety+(px_int)(btm),Color);
						}
						else
						{
							PX_GeoDrawRect(psurface,offsetx+(px_int)x,offsety+(px_int)y,offsetx+(px_int)(x+w),offsety+(px_int)(btm),Color);
						}
					}



				}
				if(Horizontal[i]>pcd->HorizontalRangeMax)
					break;
			}
		}
		break;
	case PX_OBJECT_OSCILLOSCOPE_LINEMODE_LINES:
	default:
		{
			for (i=0;i<Size;i++)
			{

				if (Horizontal[i]>=pcd->HorizontalRangeMin)
				{

					if(i>0){
						px_point pt1,pt2;

						dx1=PX_Object_OscilloscopeMapHorizontalValueToPixel(pObject,Horizontal[i-1]);
						dy1=PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,Vertical[i-1]/n,Map);
						dx2=PX_Object_OscilloscopeMapHorizontalValueToPixel(pObject,Horizontal[i]);
						dy2=PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,Vertical[i]/n,Map);

						if(PX_isLineCrossRect(PX_POINT((px_float)dx1,(px_float)dy1,0),PX_POINT((px_float)dx2,(px_float)dy2,0),PX_RECT((px_float)pcd->LeftSpacer,(px_float)pcd->TopSpacer,(px_float)PX_Object_OscilloscopeGetOscilloscopeWidth(pObject),(px_float)PX_Object_OscilloscopeGetOscilloscopeHeight(pObject)),&pt1,&pt2))
						{
							PX_GeoDrawLine(psurface,(px_int)(offsetx+pt1.x),(px_int)(offsety+pt1.y),(px_int)(offsetx+pt2.x),(px_int)(offsety+pt2.y),linewidth,Color);
						}

					}
				}
				if(Horizontal[i]>pcd->HorizontalRangeMax)
				{
					if(i>0){
						px_point pt1,pt2;

						dx1=PX_Object_OscilloscopeMapHorizontalValueToPixel(pObject,Horizontal[i-1]);
						dy1=PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,Vertical[i-1]/n,Map);
						dx2=PX_Object_OscilloscopeMapHorizontalValueToPixel(pObject,Horizontal[i]);
						dy2=PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,Vertical[i]/n,Map);
						if(PX_isLineCrossRect(PX_POINT((px_float)dx1,(px_float)dy1,0),PX_POINT((px_float)dx2,(px_float)dy2,0),PX_RECT((px_float)pcd->TopSpacer,(px_float)pcd->LeftSpacer,(px_float)PX_Object_OscilloscopeGetOscilloscopeWidth(pObject),(px_float)PX_Object_OscilloscopeGetOscilloscopeHeight(pObject)),&pt1,&pt2))
							PX_GeoDrawLine(psurface,(px_int)(offsetx+pt1.x),(px_int)(offsety+pt1.y),(px_int)(offsetx+pt2.x),(px_int)(offsety+pt2.y),linewidth,Color);
					}
					break;
				}
			}
		}
		break;

	}
}


static px_void PX_Object_OscilloscopeDrawData(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_int i;
	if(pcd->bDataUpdatePainter)
	{
		for ( i=0;i<pcd->vData.size;i++)
		{
			PX_Object_OscilloscopeData *pData=PX_VECTORAT(PX_Object_OscilloscopeData,&pcd->vData,i);
			if(pData->Visibled)
			{
				if (pData->Normalization==0)
				{
					PX_Object_OscilloscopeDrawDataInfo(psurface,pObject,pData->MapHorizontalArray,1.0,pData->MapVerticalArray,pData->linewidth,pData->Size,pData->Map,pData->Color);
				}
				else
				{
					PX_Object_OscilloscopeDrawDataInfo(psurface,pObject,pData->MapHorizontalArray,pData->Normalization,pData->MapVerticalArray,pData->linewidth,pData->Size,pData->Map,pData->Color);
				}
			}

		}
	}
}
static px_void PX_Object_OscilloscopeDrawScaleDraging(px_surface *psurface,PX_Object *pObject)
{
	px_int offsetx;
	px_int offsety;
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	if (!pcd->bScaleDrag)
	{
		return;
	}

	if(pcd->DragingPoint.y<pcd->DragStartPoint.y&&pcd->DragingPoint.x<pcd->DragStartPoint.x)
		PX_GeoDrawRect(psurface,offsetx+(px_int)pcd->DragStartPoint.x,offsety+(px_int)pcd->DragStartPoint.y,offsetx+(px_int)pcd->DragingPoint.x,offsety+(px_int)pcd->DragingPoint.y,PX_COLOR(96,171,81,128));	
	else
		PX_GeoDrawRect(psurface,offsetx+(px_int)pcd->DragStartPoint.x,offsety+(px_int)pcd->DragStartPoint.y,offsetx+(px_int)pcd->DragingPoint.x,offsety+(px_int)pcd->DragingPoint.y,PX_COLOR(96,167,233,128));

}

static px_void PX_Object_OscilloscopeDrawHelpLine(px_surface *psurface,PX_Object *pObject)
{
	px_int x,y;
	px_double value;
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!pcd->ShowHelpLine)
	{
		return;
	}
	//Draw X line


	value=PX_Object_OscilloscopeMapPixelValueToVertical(pObject,pcd->helpLineX,PX_OBJECT_OSCILLOSCOPEDATA_MAP_HORIZONTAL);

	if (value>pcd->HorizontalRangeMin&&value<pcd->HorizontalRangeMax)
	{
		px_int IsFloat;
		px_int X,Y;
		px_double ValInc=value;
		px_char text[16];
		x=PX_Object_OscilloscopeMapHorizontalValueToPixel(pObject,value);

		PX_GeoDrawLine(psurface,(px_int)objx+x,(px_int)(objy+objHeight-pcd->BottomSpacer),(px_int)objx+x,(px_int)objy+pcd->TopSpacer,(px_int)1,pcd->helpLineColor);


		//Draw text

		if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
		{
			IsFloat=PX_FALSE;
		}
		else
		{
			IsFloat=PX_TRUE;
		}


		X=PX_Object_OscilloscopeMapHorizontalValueToPixel(pObject,value);
		Y=(px_int)(objHeight-pcd->BottomSpacer);

		if(IsFloat)
			PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_FLOAT((px_float)ValInc));
		else
			PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_INT((px_int)ValInc));

		PX_FontModuleDrawText(psurface,PX_NULL,(px_int)(objx+X-PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLAGTEXT_SPACER*1.5),(px_int)objy+Y,PX_ALIGN_LEFTTOP,text,pcd->helpLineColor);

	}



	//Draw YL line


	value=PX_Object_OscilloscopeMapPixelValueToVertical(pObject,pcd->helpLineY,PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT);

	if (value>pcd->LeftVerticalRangeMin&&value<pcd->LeftVerticalRangeMax)
	{
		px_int IsFloat;
		px_int X,Y;
		px_double ValInc=value;
		px_char text[16];

		y=pcd->helpLineY;//PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,value,PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT);

		PX_GeoDrawLine(psurface,(px_int)objx+pcd->LeftSpacer,(px_int)objy+y,(px_int)(objx+objWidth-pcd->RightSpacer),(px_int)objy+y,1,pcd->helpLineColor);
		//Draw text

		if (PX_ABS(ValInc-(px_int)ValInc)<0.000000001)
		{
			IsFloat=PX_FALSE;
		}
		else
		{
			IsFloat=PX_TRUE;
		}


		X=pcd->LeftSpacer;
		Y=y;//PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,ValInc,PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT);

		if(IsFloat)
			PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_FLOAT((px_float)ValInc));
		else
			PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_INT((px_int)ValInc));

		PX_FontModuleDrawText(psurface,PX_NULL,(px_int)objx+X,(px_int)objy+Y-PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLAGTEXT_SPACER*2-1,PX_ALIGN_LEFTTOP,text,pcd->helpLineColor);

	}


	//Draw YR line

	value=PX_Object_OscilloscopeMapPixelValueToVertical(pObject,pcd->helpLineY,PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT);

	if (value>pcd->RightVerticalRangeMin&&value<pcd->RightVerticalRangeMax)
	{
		px_int IsFloat;
		px_int X,Y;
		px_double ValInc=value;
		px_char text[16];

		y=pcd->helpLineY;//PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,pflgl->Y,PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT);
		PX_GeoDrawLine(psurface,(px_int)objx+pcd->LeftSpacer,(px_int)objy+y,(px_int)(objx+objWidth-pcd->RightSpacer),(px_int)objy+y,(px_int)(1),pcd->helpLineColor);


		if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
		{
			IsFloat=PX_FALSE;
		}
		else
		{
			IsFloat=PX_TRUE;
		}

		X=(px_int)(objWidth-pcd->RightSpacer);
		Y=y;//PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,ValInc,PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT);

		if(IsFloat)
			PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_FLOAT((px_float)ValInc));
		else
			PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_INT((px_int)ValInc));

		PX_FontModuleDrawText(psurface,PX_NULL,(px_int)objx+X,(px_int)objy+Y-PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLAGTEXT_SPACER*2-1,PX_ALIGN_LEFTTOP,text,pcd->helpLineColor);

	}
}



static px_void PX_Object_OscilloscopeDrawFlagLine(px_surface *psurface,PX_Object *pObject)
{
	px_int x,y,i;
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	for (i=0;i<pcd->vFlagLine.size;i++)
	{
		//Draw X line
		PX_Object_OscilloscopeFlagLine *pflgl=PX_VECTORAT(PX_Object_OscilloscopeFlagLine,&pcd->vFlagLine,i);
		if(pflgl->XYshow&PX_OBJECT_OSCILLOSCOPEFLAGLINE_XSHOW)
		{
			if (pflgl->X>=pcd->HorizontalRangeMin&&pflgl->X<=pcd->HorizontalRangeMax)
			{
				px_int IsFloat;
				px_int X,Y;
				px_double ValInc=pflgl->X;
				px_char text[16];
				x=PX_Object_OscilloscopeMapHorizontalValueToPixel(pObject,pflgl->X);

				PX_GeoDrawLine(psurface,(px_int)objx+x,(px_int)((px_int)objy+objHeight-pcd->BottomSpacer),(px_int)objx+x,(px_int)objy+pcd->TopSpacer,(px_int)pflgl->LineWidth,pflgl->color);
				//Draw text

				if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
				{
					IsFloat=PX_FALSE;
				}
				else
				{
					IsFloat=PX_TRUE;
				}


				X=PX_Object_OscilloscopeMapHorizontalValueToPixel(pObject,pflgl->X);
				Y=(px_int)(objHeight-pcd->BottomSpacer);

				if(IsFloat)
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_FLOAT((px_float)ValInc));
				else
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_INT((px_int)ValInc));

				PX_FontModuleDrawText(psurface,PX_NULL,(px_int)((px_int)objx+X-PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLAGTEXT_SPACER*1.5),(px_int)objy+Y,PX_ALIGN_LEFTTOP,text,pcd->FontColor);
			}
		}


		//Draw YL line

		if(pflgl->XYshow&PX_OBJECT_OSCILLOSCOPEFLAGLINE_XSHOW)
		{
			if (pflgl->Y>=pcd->LeftVerticalRangeMin&&pflgl->Y<=pcd->LeftVerticalRangeMax)
			{
				px_int IsFloat;
				px_int X,Y;
				px_double ValInc=pflgl->Y;
				px_char text[16];
				y=PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,pflgl->Y,PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT);

				PX_GeoDrawLine(psurface,(px_int)objx+pcd->LeftSpacer,(px_int)objy+y,(px_int)((px_int)objx+objWidth-pcd->RightSpacer),(px_int)objy+y,(px_int)(pflgl->LineWidth),pflgl->color);
				//Draw text

				if (PX_ABS(ValInc-(px_int)ValInc)<0.000000001)
				{
					IsFloat=PX_FALSE;
				}
				else
				{
					IsFloat=PX_TRUE;
				}


				X=pcd->LeftSpacer;
				Y=y;//PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,ValInc,PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT);

				if(IsFloat)
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_FLOAT((px_float)ValInc));
				else
					PX_sprintf1(text,sizeof(text),"%1",PX_STRINGFORMAT_INT((px_int)ValInc));

				PX_FontModuleDrawText(psurface,PX_NULL,(px_int)objx+X-4*PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLAGTEXT_SPACER,(px_int)objy+Y-PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLAGTEXT_SPACER,PX_ALIGN_LEFTTOP,text,pcd->FontColor);
			}
		}


		//Draw YR line
		if(pflgl->XYshow&PX_OBJECT_OSCILLOSCOPEFLAGLINE_YRSHOW)
		{
			if (pflgl->Y>=pcd->RightVerticalRangeMin&&pflgl->Y<=pcd->RightVerticalRangeMax)
			{
				px_int IsFloat;
				px_int X,Y;
				px_double ValInc=pflgl->Y;
				px_char text[16];

				y=PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,pflgl->Y,PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT);
				PX_GeoDrawLine(psurface,(px_int)objx+pcd->LeftSpacer,(px_int)objy+y,(px_int)((px_int)objx+objWidth-pcd->RightSpacer),(px_int)objy+y,(px_int)(pflgl->LineWidth),pflgl->color);

				if (PX_ABS(ValInc-(px_int)ValInc)<0.000001f)
				{
					IsFloat=PX_FALSE;
				}
				else
				{
					IsFloat=PX_TRUE;
				}

				X=(px_int)(objWidth-pcd->RightSpacer);
				Y=y;//PX_Object_OscilloscopeMapVerticalValueToPixel(pObject,ValInc,PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT);

				PX_FontModuleDrawText(psurface,PX_NULL,(px_int)objx+X,(px_int)objy+Y-PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLAGTEXT_SPACER,PX_ALIGN_LEFTTOP,text,pcd->FontColor);
			}
		}
	}
}


px_void PX_Object_OscilloscopeDrawMarkLine(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);

	if (pcd->MarkValueEnabled&&pcd->OnMarkStatus)
	{
		if(pcd->MarkLineX!=-1)
		{
			PX_GeoDrawLine(psurface,pcd->MarkLineX,pcd->TopSpacer,pcd->MarkLineX,(px_int)(pObject->Height-pcd->BottomSpacer),1,PX_COLOR(255,255,0,0));
		}
	}
}



px_void PX_Object_OscilloscopeRestoreOscilloscope(PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd->bScaling)
	{
		return;
	}
	PX_Object_OscilloscopeSetHorizontalMin(pObject,pcd->ResHorizontalRangeMin);
	PX_Object_OscilloscopeSetHorizontalMax(pObject,pcd->ResHorizontalRangeMax);
	PX_Object_OscilloscopeSetLeftVerticalMin(pObject,pcd->ResLeftVerticalRangeMin);
	PX_Object_OscilloscopeSetLeftVerticalMax(pObject,pcd->ResLeftVerticalRangeMax);
	PX_Object_OscilloscopeSetRightVerticalMin(pObject,pcd->ResRightVerticalRangeMin);
	PX_Object_OscilloscopeSetRightVerticalMax(pObject,pcd->ResRightVerticalRangeMax);
	pcd->bScaling=PX_FALSE;
}
px_void PX_Object_OscilloscopeScaleOscilloscope(PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_double Min,Max;
	px_double dMin;
	px_double dMax;
	if(!pcd->ScaleEnabled)
	{
		return;
	}

	if (pcd->DragingPoint.x==pcd->DragStartPoint.x)
	{
		return;
	}
	if (pcd->DragingPoint.y==pcd->DragStartPoint.y)
	{
		return;
	}
	if (!pcd->bScaling)
	{
		pcd->ResHorizontalRangeMax=pcd->HorizontalRangeMax;
		pcd->ResHorizontalRangeMin=pcd->HorizontalRangeMin;

		pcd->ResLeftVerticalRangeMax=pcd->LeftVerticalRangeMax;
		pcd->ResLeftVerticalRangeMin=pcd->LeftVerticalRangeMin;
		pcd->ResRightVerticalRangeMin=pcd->RightVerticalRangeMin;
		pcd->ResRightVerticalRangeMax=pcd->RightVerticalRangeMax;

		pcd->bScaling=PX_TRUE;
	}


	if (pcd->DragStartPoint.x<pcd->DragingPoint.x)
	{
		Min=pcd->DragStartPoint.x;
		Max=pcd->DragingPoint.x;
	}
	else
	{
		Max=pcd->DragStartPoint.x;
		Min=pcd->DragingPoint.x;
	}
	dMin=PX_Object_OscilloscopeMapPixelValueToHorizontal(pObject,(px_int)Min);
	dMax=PX_Object_OscilloscopeMapPixelValueToHorizontal(pObject,(px_int)Max);

	PX_Object_OscilloscopeSetHorizontalMin(pObject,dMin);
	PX_Object_OscilloscopeSetHorizontalMax(pObject,dMax);

	if (pcd->DragStartPoint.y<pcd->DragingPoint.y)
	{
		Max=pcd->DragStartPoint.y;
		Min=pcd->DragingPoint.y;
	}
	else
	{
		Min=pcd->DragStartPoint.y;
		Max=pcd->DragingPoint.y;
	}

	dMin=PX_Object_OscilloscopeMapPixelValueToVertical(pObject,(px_int)Min,PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT);
	dMax=PX_Object_OscilloscopeMapPixelValueToVertical(pObject,(px_int)Max,PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT);
	PX_Object_OscilloscopeSetLeftVerticalMin(pObject,dMin);
	PX_Object_OscilloscopeSetLeftVerticalMax(pObject,dMax);

	dMin=PX_Object_OscilloscopeMapPixelValueToVertical(pObject,(px_int)Min,PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT);
	dMax=PX_Object_OscilloscopeMapPixelValueToVertical(pObject,(px_int)Max,PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT);

	PX_Object_OscilloscopeSetRightVerticalMin(pObject,dMin);
	PX_Object_OscilloscopeSetRightVerticalMax(pObject,dMax);
}


px_void PX_Object_OscilloscopeRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	if (!pcd)
	{
		PX_ASSERT();
		return;
	}

	// 
	// 	PX_Object_OscilloscopeDrawMarkLine(psurface,pObject);

	if(pcd->ShowGuides)
		PX_Object_OscilloscopeDrawDashed(psurface,pObject);

	PX_Object_OscilloscopeDrawDashText(psurface,pObject);

	PX_Object_OscilloscopeDrawTitle(psurface,pObject);

	PX_Object_OscilloscopeDrawData(psurface,pObject);

	PX_Object_OscilloscopeDrawFrameLine(psurface,pObject);

	if(pcd->ScaleEnabled)
		PX_Object_OscilloscopeDrawScaleDraging(psurface,pObject);

	PX_Object_OscilloscopeDrawHelpLine(psurface,pObject);

	PX_Object_OscilloscopeDrawFlagLine(psurface,pObject);
}

px_void PX_Object_OscilloscopeFree(PX_Object *pObject)
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	PX_VectorFree(&pcd->vData);
	PX_VectorFree(&pcd->vFlagLine);
}

px_void PX_Object_OscilloscopeCursorPressEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr)
{
	px_float x;
	px_float y;
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);
	if(!PX_ObjectIsPointInRegion(pObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
	{
		return;
	}

	if (x<pcd->LeftSpacer)
	{
		x=(px_float)pcd->LeftSpacer;
	}
	if (x>objWidth-pcd->RightSpacer)
	{
		x=(objWidth-pcd->RightSpacer);
	}
	if (y<pcd->TopSpacer)
	{
		y=(px_float)pcd->TopSpacer;
	}
	if (y>objHeight-pcd->BottomSpacer)
	{
		y=(objHeight-pcd->BottomSpacer);
	}

	pcd->DragStartPoint.x=(px_float)(x);
	pcd->DragStartPoint.y=(px_float)(y);

	pcd->DragingPoint=pcd->DragStartPoint;

	pcd->bScaleDrag=PX_TRUE;
}

px_void PX_Object_OscilloscopeCursorReleaseEvent( PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{

	px_float x=PX_Object_Event_GetCursorX(e);
	px_float y=PX_Object_Event_GetCursorY(e);

	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	pcd->OnMarkStatus=PX_FALSE;
	pcd->MarkLineX=-1;

	if (pcd->bScaleDrag)
	{
		pcd->bScaleDrag=PX_FALSE;
		pcd->DragingPoint.x=(px_float)(PX_Object_Event_GetCursorX(e)-objx);
		pcd->DragingPoint.y=(px_float)(PX_Object_Event_GetCursorY(e)-objy);

		if(pcd->DragingPoint.y<pcd->DragStartPoint.y&&pcd->DragingPoint.x<pcd->DragStartPoint.x)
		{
			PX_Object_OscilloscopeRestoreOscilloscope(pObject);
		}
		else
		{
			PX_Object_OscilloscopeScaleOscilloscope(pObject);
		}
	}

}

px_void PX_Object_OscilloscopeCursorDragEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pcd->bScaleDrag)
	{
		px_float x=(PX_Object_Event_GetCursorX(e)-objx);
		px_float y=(PX_Object_Event_GetCursorY(e)-objy);

		pcd->OnMarkStatus=PX_FALSE;
		pcd->MarkLineX=-1;

		// 	emit SignalOnMousePosition(this->MapPixelValueToHorizontal(e->x()),
		// 		e->x(),
		// 		this->MapPixelValueToVertical(e->y(),OSCILLOSCOPEDATA_MAP_RIGHT));
		if (x<pcd->LeftSpacer)
		{
			x=(px_float)pcd->LeftSpacer;
		}
		if (x>objWidth-pcd->RightSpacer)
		{
			x=(objWidth-pcd->RightSpacer);
		}
		if (y<pcd->TopSpacer)
		{ 
			y=(px_float)pcd->TopSpacer;
		}
		if (y>objHeight-pcd->BottomSpacer)
		{
			y=(objHeight-pcd->BottomSpacer);
		}

		pcd->DragingPoint.x=(px_float)(x);
		pcd->DragingPoint.y=(px_float)(y);
	}

}

px_void PX_Object_OscilloscopeCursorMoveEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	PX_Object_Oscilloscope *pcd=PX_Object_GetOscilloscope(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pcd->ShowHelpLine)
	{
		px_float x=(PX_Object_Event_GetCursorX(e)-objx);
		px_float y=(PX_Object_Event_GetCursorY(e)-objy);

		pcd->OnMarkStatus=PX_FALSE;
		pcd->MarkLineX=-1;

		// 	emit SignalOnMousePosition(this->MapPixelValueToHorizontal(e->x()),
		// 		e->x(),
		// 		this->MapPixelValueToVertical(e->y(),OSCILLOSCOPEDATA_MAP_RIGHT));
		if (x<pcd->LeftSpacer)
		{
			x=(px_float)pcd->LeftSpacer;
		}
		if (x>objWidth-pcd->RightSpacer)
		{
			x=(objWidth-pcd->RightSpacer);
		}
		if (y<pcd->TopSpacer)
		{ 
			y=(px_float)pcd->TopSpacer;
		}
		if (y>objHeight-pcd->BottomSpacer)
		{
			y=(objHeight-pcd->BottomSpacer);
		}

		if (pcd->bScaleDrag)
		{
			pcd->DragingPoint.x=(px_float)(x);
			pcd->DragingPoint.y=(px_float)(y);
		}

		pcd->helpLineX=(px_int)(x);
		pcd->helpLineY=(px_int)(y);
	}
}




PX_Object *PX_Object_OscilloscopeCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_FontModule *fontmodule)
{
	PX_Object *pObject;
	PX_Object_Oscilloscope Oscilloscope;
	PX_memset(&Oscilloscope,0,sizeof(Oscilloscope));
	Oscilloscope.mp=mp;
	Oscilloscope.MinHorizontalPixelDividing=PX_OBJECT_OSCILLOSCOPE_DEFAULE_MINHORIZONTALPIXELDIVIDING;
	Oscilloscope.MinVerticalPixelDividing=PX_OBJECT_OSCILLOSCOPE_DEFAULE_MINVERTICALPIXELDIVIDING;
	Oscilloscope.HorizontalDividing=PX_OBJECT_OSCILLOSCOPE_DEFAULE_DIVIDING;
	Oscilloscope.LeftVerticalDividing=PX_OBJECT_OSCILLOSCOPE_DEFAULE_DIVIDING;
	Oscilloscope.RightVerticalDividing=PX_OBJECT_OSCILLOSCOPE_DEFAULE_DIVIDING;
	Oscilloscope.bScaleDrag=PX_FALSE;
	Oscilloscope.guidesShowMode=PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE_ALL;
	Oscilloscope.helpLineColor=PX_COLOR(255,255,0,0);
	Oscilloscope.ShowHelpLine=PX_TRUE;

	Oscilloscope.LeftSpacer=(px_int)(1.5f*PX_OBJECT_OSCILLOSCOPE_DEFAULT_SPACER);
	Oscilloscope.RightSpacer=(px_int)(1.5*PX_OBJECT_OSCILLOSCOPE_DEFAULT_SPACER);
	Oscilloscope.TopSpacer=PX_OBJECT_OSCILLOSCOPE_DEFAULT_SPACER;
	Oscilloscope.BottomSpacer=PX_OBJECT_OSCILLOSCOPE_DEFAULT_SPACER;

	Oscilloscope.HorizontalRangeMin=-150;
	Oscilloscope.HorizontalRangeMax=150;

	Oscilloscope.LeftVerticalRangeMin=0;
	Oscilloscope.LeftVerticalRangeMax=100;

	Oscilloscope.RightVerticalRangeMax=1.0;
	Oscilloscope.RightVerticalRangeMin=0;

	Oscilloscope.FloatFlagFormat_H=PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLOAT_FLAGFORMAT_H;
	Oscilloscope.IntFlagFormat_H=PX_OBJECT_OSCILLOSCOPE_DEFAULT_INT_FLAGFORMAT_H;

	Oscilloscope.FloatFlagFormat_L=PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLOAT_FLAGFORMAT_L;
	Oscilloscope.IntFlagFormat_L=PX_OBJECT_OSCILLOSCOPE_DEFAULT_INT_FLAGFORMAT_L;

	Oscilloscope.FloatFlagFormat_R=PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLOAT_FLAGFORMAT_R;
	Oscilloscope.IntFlagFormat_R=PX_OBJECT_OSCILLOSCOPE_DEFAULT_INT_FLAGFORMAT_R;

	Oscilloscope.Exponential_Format=PX_OBJECT_OSCILLOSCOPE_DEFAULT_EXPONENTIAL_FORMAT;

	Oscilloscope.bScaling=PX_FALSE;

	Oscilloscope.FontSize=PX_OBJECT_OSCILLOSCOPE_DEFAULT_FONT_SIZE;
	Oscilloscope.DataLineWidth=PX_OBJECT_OSCILLOSCOPE_DEFAULT_LINE_WIDTH;
	Oscilloscope.FontColor= PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	Oscilloscope.borderColor= PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	Oscilloscope.DashColor=PX_COLOR(PX_OBJECT_OSCILLOSCOPE_DEFAULT_DASH_RGB);
	Oscilloscope.OnMarkStatus=PX_FALSE;
	Oscilloscope.MarkValueEnabled=PX_TRUE;
	Oscilloscope.MarkLineX=-1;

	Oscilloscope.bDataUpdatePainter=PX_TRUE;

	Oscilloscope.LineMode=PX_OBJECT_OSCILLOSCOPE_LINEMODE_LINES;

	Oscilloscope.LeftTextShow=PX_TRUE;
	Oscilloscope.RightTextShow=PX_TRUE;
	Oscilloscope.HorizontalTextShow=PX_TRUE;
	Oscilloscope.ScaleEnabled=PX_TRUE;
	Oscilloscope.ShowGuides=PX_TRUE;

	Oscilloscope.LeftTitle="";
	Oscilloscope.RightTitle="";
	Oscilloscope.TopTitle="";
	Oscilloscope.BottomTitle="";
	Oscilloscope.fontmodule=fontmodule;
	Oscilloscope.leftTextDisplayMode=PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE_NORMAL;
	Oscilloscope.RightTextDisplayMode=PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE_NORMAL;

	PX_VectorInitialize(mp,&Oscilloscope.vData,sizeof(PX_Object_OscilloscopeData),16);
	PX_VectorInitialize(mp,&Oscilloscope.vFlagLine,sizeof(PX_Object_OscilloscopeFlagLine),16);

	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0,PX_OBJECT_TYPE_OSCILLOSCOPE,PX_Object_OscilloscopeUpdate,PX_Object_OscilloscopeRender,PX_Object_OscilloscopeFree,&Oscilloscope,sizeof(PX_Object_Oscilloscope));

	//PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_OscilloscopeCursorMoveEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_OscilloscopeCursorMoveEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_OscilloscopeCursorDragEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_OscilloscopeCursorPressEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_OscilloscopeCursorReleaseEvent,PX_NULL);

	return pObject;
}

PX_Object_FilterEditor * PX_Object_GetFilterEditor(PX_Object *Object)
{
	if (Object->Type==PX_OBJECT_TYPE_FILTEREDITOR)
	{
		return (PX_Object_FilterEditor *)Object->pObject;
	}
	return PX_NULL;
}

static px_void PX_Object_FilterEditorDrawFrameLine(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_int offsetx;
	px_int offsety;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	PX_GeoDrawLine(psurface,offsetx,offsety,offsetx+(px_int)(objWidth-1),offsety,PX_OBJECT_FILTEREDITOR_DEFAULT_FRAMELINE_WIDTH,pfe->borderColor);
	PX_GeoDrawLine(psurface,offsetx,offsety,offsetx,offsety+(px_int)(objHeight-1),PX_OBJECT_FILTEREDITOR_DEFAULT_FRAMELINE_WIDTH,pfe->borderColor);
	PX_GeoDrawLine(psurface,(px_int)(offsetx+objWidth-1),offsety+(px_int)(objHeight-1),offsetx,offsety+(px_int)objHeight-1,PX_OBJECT_OSCILLOSCOPE_DEFAULT_FRAMELINE_WIDTH,pfe->borderColor);
	PX_GeoDrawLine(psurface,(px_int)(offsetx+objWidth-1),offsety+(px_int)(objHeight-1),offsetx+(px_int)(objWidth-1),offsety,PX_OBJECT_OSCILLOSCOPE_DEFAULT_FRAMELINE_WIDTH,pfe->borderColor);
}

static px_void PX_Object_FilterEditorDrawSelectDraging(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_int offsetx;
	px_int offsety;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	offsetx=(px_int)objx;
	offsety=(px_int)objy;

	if (!pfe->bSelectDrag)
	{
		return;
	}

	PX_GeoDrawRect(psurface,(px_int)(offsetx+pfe->DragStartPoint.x),(px_int)(offsety+(px_int)pfe->DragStartPoint.y),(px_int)(offsetx+(px_int)pfe->DragingPoint.x),(px_int)(offsety+(px_int)pfe->DragingPoint.y),PX_COLOR(96,171,81,128));	
}

static px_void PX_Object_FilterEditorDrawPt(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_int i;
	px_float oftx;
	px_float ofty;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	oftx=objx;
	ofty=objy;

	for (i=0;i<pfe->opCount;i++)
	{
		PX_GeoDrawSolidCircle(psurface,(px_int)(oftx+pfe->pt[i].x),(px_int)(ofty+pfe->pt[i].y),pfe->radius,pfe->ptColor);
		if (pfe->pt[i].bselect||pfe->pt[i].bCursor)
		{
			PX_GeoDrawCircle(psurface,(px_int)(oftx+pfe->pt[i].x),(px_int)(ofty+pfe->pt[i].y),pfe->radius+2,1,pfe->ptColor);
		}
	}

	for (i=0;i<pfe->opCount-1;i++)
	{
		PX_GeoDrawLine(psurface,(px_int)(oftx+pfe->pt[i].x),(px_int)(ofty+pfe->pt[i].y),(px_int)(oftx+pfe->pt[i+1].x),(px_int)(ofty+pfe->pt[i+1].y),2,pfe->ptColor);
	}

}

static px_void PX_Object_FilterEditorDrawHelpLine(px_surface *psurface,PX_Object *pObject)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_char text[16];
	px_double midy,incx,incy,x,y,val;
	px_double oftx,ofty;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	if (!pfe->ShowHelpLine)
	{
		return;
	}
	oftx=objx;
	ofty=objy;
	midy=objHeight/2;
	incy=objHeight/2/(pfe->VerticalDividing/2);

	//up
	for (y=midy;y>=0;y-=incy)
	{
		//line
		PX_GeoDrawLine(psurface,(px_int)(oftx),(px_int)(ofty+y),(px_int)(oftx+objWidth),(px_int)(ofty+y),1,pfe->helpLineColor);
		val=(midy-y)/(objHeight/2)*pfe->rangedb;
		switch (pfe->FilterType)
		{
		case PX_OBJECT_FILTER_TYPE_dB:
			PX_sprintf1(text,sizeof(text),"+%1.2db",PX_STRINGFORMAT_FLOAT((px_float)val));
			break;
		case PX_OBJECT_FILTER_TYPE_Liner:
			PX_sprintf1(text,sizeof(text),"+%1.2",PX_STRINGFORMAT_FLOAT((px_float)val));
			break;
		}

		//text
		PX_FontModuleDrawText(psurface,PX_NULL,(px_int)(oftx-1),(px_int)(ofty+y-5),PX_ALIGN_RIGHTTOP,text,pfe->FontColor);
	}
	//down
	for (y=midy;y<=objHeight;y+=incy)
	{
		//line
		PX_GeoDrawLine(psurface,(px_int)(oftx),(px_int)(ofty+y),(px_int)(oftx+objWidth),(px_int)(ofty+y),1,pfe->helpLineColor);
		val=(midy-y)/(objHeight/2)*pfe->rangedb;
		switch (pfe->FilterType)
		{
		case PX_OBJECT_FILTER_TYPE_dB:
			PX_sprintf1(text,sizeof(text),"%1.2db",PX_STRINGFORMAT_FLOAT((px_float)val));
			break;
		case PX_OBJECT_FILTER_TYPE_Liner:
			PX_sprintf1(text,sizeof(text),"%1.2",PX_STRINGFORMAT_FLOAT((px_float)val));
			break;
		}
		//text
		PX_FontModuleDrawText(psurface,PX_NULL,(px_int)(oftx-1),(px_int)(ofty+y-5),PX_ALIGN_RIGHTTOP,text,pfe->FontColor);
	}

	//horizontal
	incx=objWidth/pfe->HorizontalDividing;
	for (x=0;x<objWidth;x+=incx)
	{
		//line
		PX_GeoDrawLine(psurface,(px_int)(oftx+x),(px_int)(ofty+0),(px_int)(oftx+x),(px_int)(ofty+objHeight-1),1,pfe->helpLineColor);
		val=x/objWidth;
		//text
		if (pfe->showHorizontal)
		{
			PX_sprintf1(text,sizeof(text),"%1.2",PX_STRINGFORMAT_FLOAT((px_float)val));
			PX_FontModuleDrawText(psurface,PX_NULL,(px_int)(oftx+x-1),(px_int)(ofty+objHeight+3),PX_ALIGN_CENTER,text,pfe->FontColor);
		}
	}

}

px_void PX_Object_FilterEditorCursorReleaseEvent( PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);

	pfe->bSelectDrag=PX_FALSE;
	pfe->bAdjust=PX_FALSE;
}


px_void PX_Object_FilterEditorRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	PX_Object_FilterEditorDrawFrameLine(psurface,pObject);
	PX_Object_FilterEditorDrawHelpLine(psurface,pObject);
	PX_Object_FilterEditorDrawPt(psurface,pObject);
	PX_Object_FilterEditorDrawSelectDraging(psurface,pObject);
}

px_void PX_Object_FilterEditorCursorPressEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr)
{
	px_int i,j;
	px_bool bSelectlge1=PX_FALSE;
	px_float x;
	px_float y;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);


	if(!PX_ObjectIsPointInRegion(pObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
	{
		return;
	}

	for (i=0;i<pfe->opCount;i++)
	{
		if(PX_isPointInCircle(PX_POINT(x,y,0),PX_POINT((px_float)pfe->pt[i].x,(px_float)pfe->pt[i].y,0),(px_float)pfe->radius))
		{
			if (!pfe->pt[i].bselect)
			{
				for (j=0;j<pfe->opCount;j++)
				{
					pfe->pt[j].bselect=PX_FALSE;
				}
				pfe->pt[i].bselect=PX_TRUE;
			}
			bSelectlge1=PX_TRUE;
		}

	}

	if (bSelectlge1)
	{
		pfe->bSelectDrag=PX_FALSE;
		pfe->bAdjust=PX_TRUE;
		pfe->lastAdjustPoint.x=(x);
		pfe->lastAdjustPoint.y=(y);
	}
	else
	{
		for (i=0;i<pfe->opCount;i++)
		{
			pfe->pt[i].bselect=PX_FALSE;
		}
		pfe->DragStartPoint.x=(x);
		pfe->DragStartPoint.y=(y);
		pfe->DragingPoint=pfe->DragStartPoint;
		pfe->bSelectDrag=PX_TRUE;
		pfe->bAdjust=PX_FALSE;
	}

}

px_void PX_Object_FilterEditorCursorMoveEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_int i;
	px_float x,y;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);


	if(!PX_ObjectIsPointInRegion(pObject,PX_Object_Event_GetCursorX(e),PX_Object_Event_GetCursorY(e)))
	{
		return;
	}

	for (i=0;i<pfe->opCount;i++)
	{
		if(PX_isPointInCircle(PX_POINT(x,y,0),PX_POINT((px_float)pfe->pt[i].x,(px_float)pfe->pt[i].y,0),(px_float)pfe->radius))
		{
			pfe->pt[i].bCursor=PX_TRUE;
		}
		else
		{
			pfe->pt[i].bCursor=PX_FALSE;
		}
	}

}


px_void PX_Object_FilterEditorCursorDragEvent(PX_Object *pObject, PX_Object_Event e,px_void *ptr )
{
	px_int i;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_float x,y;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	x=(PX_Object_Event_GetCursorX(e)-objx);
	y=(PX_Object_Event_GetCursorY(e)-objy);

	if (pfe->bSelectDrag)
	{
		px_rect rect;

		if (x<0)
		{
			x=0;
		}
		if (x>objWidth)
		{
			x=(objWidth);
		}
		if (y<0)
		{ 
			y=0;
		}
		if (y>objHeight)
		{
			y=(objHeight);
		}

		pfe->DragingPoint.x=(px_float)(x);
		pfe->DragingPoint.y=(px_float)(y);
		rect=PX_RECT(pfe->DragStartPoint.x,pfe->DragStartPoint.y,x-pfe->DragStartPoint.x,y-pfe->DragStartPoint.y);
		for (i=0;i<pfe->opCount;i++)
		{
			if(PX_isPointInRect(PX_POINT((px_float)pfe->pt[i].x,(px_float)pfe->pt[i].y,0),rect))
			{
				pfe->pt[i].bselect=PX_TRUE;
			}
			else
			{
				pfe->pt[i].bselect=PX_FALSE;
			}
		}
	}

	else if (pfe->bAdjust)
	{
		for (i=0;i<pfe->opCount;i++)
		{
			if(pfe->pt[i].bselect==PX_TRUE)
			{
				pfe->pt[i].y+=(px_int)(y-pfe->lastAdjustPoint.y);
				if (pfe->pt[i].y<0)
				{
					pfe->pt[i].y=0;
				}
				if (pfe->pt[i].y>objHeight-1)
				{
					pfe->pt[i].y=(px_int)objHeight-1;
				}
			}
		}
		pfe->lastAdjustPoint=PX_POINT(x,y,0);
	}
}


px_void PX_Object_FilterEditorSetOperateCount(PX_Object *pObject,px_int count)
{
	px_int i;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	px_float Inc;
	px_int y;
	if (count<2)
	{
		count=2;
	}
	if (count>PX_COUNTOF(pfe->pt))
	{
		count=PX_COUNTOF(pfe->pt);
	}
	Inc=pObject->Width/(count-1);
	y=(px_int)(pObject->Height/2);
	for (i=0;i<count;i++)
	{
		pfe->pt[i].x=(px_int)(Inc*i);
		pfe->pt[i].y=y;
	}
	pfe->opCount=count;
}

px_void PX_Object_FilterEditorSetType(PX_Object *Object,PX_OBJECT_FILTER_TYPE type)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->FilterType=type;
	}
}

px_void PX_Object_FilterEditorSetHorizontalShow(PX_Object *Object,px_bool HorizontalShow)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->showHorizontal=HorizontalShow;
	}
}

px_void PX_Object_FilterEditorReset(PX_Object *Object)
{
	px_int i,y;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	y=(px_int)(Object->Height/2);
	for (i=0;i<pfe->opCount;i++)
	{
		pfe->pt[i].y=y;
	}
}

px_void PX_Object_FilterEditorSetRange(PX_Object *Object,px_double range)
{

	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->rangedb=range;
	}
}

px_void PX_Object_FilterEditorSetFontColor(PX_Object *Object,px_color clr)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->FontColor=clr;
	}
}

px_void PX_Object_FilterEditorSetBorderColor(PX_Object *Object,px_color clr)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->borderColor=clr;
	}
}

px_void PX_Object_FilterEditorSethelpLineColor(PX_Object *Object,px_color clr)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->helpLineColor=clr;
	}
}

px_void PX_Object_FilterEditorSetFontSize(PX_Object *Object,px_int size)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->FontSize=size;
	}
}

px_void PX_Object_FilterEditorSetHorizontalDividing(PX_Object *Object,px_int div)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->HorizontalDividing=div;
	}
}

px_void PX_Object_FilterEditorSetVerticalDividing(PX_Object *Object,px_int div)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (pfe)
	{
		pfe->VerticalDividing=div;
	}
}

px_void PX_Object_FilterEditorMapData(PX_Object *Object,px_double data[],px_int size)
{
	px_int i,mapIndex;
	px_double ptValue[PX_OBJECT_FILTER_EDITOR_MAX_PT];
	px_double step,frac,d2,d1,dm;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	if (!pfe)
	{
		return;
	}
	//pt to value
	for (i=0;i<pfe->opCount;i++)
	{
		if (pfe->pt[i].y>Object->Height-10)
		{
			ptValue[i]=-1000;
		}
		else
		{
			ptValue[i]=((Object->Height/2)-pfe->pt[i].y)/(Object->Height/2)*pfe->rangedb;
		}
	}

	step=1.0/(pfe->opCount-1);

	for (i=0;i<size;i++)
	{
		mapIndex=(px_int)((1.0*i/size)/step);
		frac=((1.0*i/size)-mapIndex*step)/step;
		d2=ptValue[mapIndex+1];
		d1=ptValue[mapIndex];
		dm=d1+frac*(d2-d1);
		switch(pfe->FilterType)
		{
		case PX_OBJECT_FILTER_TYPE_dB:
			dm=PX_pow(10,dm/20.0);
			break;
		case PX_OBJECT_FILTER_TYPE_Liner:
			break;
		}
		data[i]=dm;
	}
}

px_double PX_Object_FilterEditorMapValue(PX_Object *pObject,px_double precent)
{
	px_int mapIndex;
	px_double step,frac,d2,d1,dm;
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(pObject);
	if (!pfe)
	{
		return 1;
	}

	step=1.0/(pfe->opCount-1);
	mapIndex=(px_int)(precent/step);
	if (mapIndex>pfe->opCount-1)
	{
		return 1;
	}
	frac=(precent-mapIndex*step)/step;
	d2=((pObject->Height/2)-pfe->pt[mapIndex+1].y)/(pObject->Height/2)*pfe->rangedb;
	d1=((pObject->Height/2)-pfe->pt[mapIndex].y)/(pObject->Height/2)*pfe->rangedb;
	dm=d1+frac*(d2-d1);
	switch(pfe->FilterType)
	{
	case PX_OBJECT_FILTER_TYPE_dB:
		dm=PX_pow(10,dm/20.0);
		break;
	case PX_OBJECT_FILTER_TYPE_Liner:
		break;
	}
	return dm;
}

PX_Object * PX_Object_FilterEditorCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_OBJECT_FILTER_TYPE type)
{
	PX_Object *pObject;
	PX_Object_FilterEditor FilterEditor;
	PX_memset(&FilterEditor,0,sizeof(FilterEditor));
	FilterEditor.showHorizontal=PX_TRUE;
	FilterEditor.borderColor=PX_COLOR(255,0,0,0);
	FilterEditor.FontColor=PX_COLOR(255,0,0,0);
	FilterEditor.bSelectDrag=PX_FALSE;
	FilterEditor.bAdjust=PX_FALSE;
	FilterEditor.DragingPoint=PX_POINT(0,0,0);
	FilterEditor.DragStartPoint=PX_POINT(0,0,0);
	FilterEditor.FontSize=16;
	FilterEditor.helpLineColor=PX_COLOR(255,0,0,0);
	FilterEditor.ptColor=PX_COLOR(255,0,0,0);
	FilterEditor.HorizontalDividing=16;
	FilterEditor.VerticalDividing=16;
	FilterEditor.ShowHelpLine=PX_TRUE;
	FilterEditor.showHorizontal=PX_FALSE;
	FilterEditor.FilterType=type;
	FilterEditor.rangedb=6;
	FilterEditor.radius=PX_OBJECT_FILTER_EDITOR_DEFAULT_RADIUS;
	pObject=PX_ObjectCreateEx(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0,PX_OBJECT_TYPE_FILTEREDITOR,PX_NULL,PX_Object_FilterEditorRender,PX_NULL,&FilterEditor,sizeof(PX_Object_FilterEditor));
	PX_Object_FilterEditorSetOperateCount(pObject,8);

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_FilterEditorCursorMoveEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_FilterEditorCursorDragEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_FilterEditorCursorPressEvent,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_FilterEditorCursorReleaseEvent,PX_NULL);
	return pObject;
}

px_void PX_Object_FilterEditorSetborderColor(PX_Object *Object,px_color clr)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	pfe->borderColor=clr;
}



px_void PX_Object_FilterEditorSetptColor(PX_Object *Object,px_color clr)
{
	PX_Object_FilterEditor *pfe=PX_Object_GetFilterEditor(Object);
	pfe->ptColor=clr;
}

