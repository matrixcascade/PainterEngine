#include "PX_Object_Sliderbar.h"

px_void PX_Object_SliderBarOnMouseLButtonDown(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pObject);
	px_float x,y;
	px_int SliderBtnLen,Sx,Sy;
	px_rect rect;
	px_int Range,relValue;

	px_int objx,objy,objw,objh;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(px_int)(pObject->x+inheritX);
	objy=(px_int)(pObject->y+inheritY);
	objw=(px_int)pObject->Width;
	objh=(px_int)pObject->Height;

	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);


	if (pSliderBar)
	{
		Range=pSliderBar->Max-pSliderBar->Min;
		relValue=pSliderBar->Value-pSliderBar->Min;

		if(PX_ObjectIsPointInRegion(pObject,(px_float)x,(px_float)y))
		{
			if (pSliderBar==PX_NULL)
			{
				return;
			}

			if (objw<=10)
			{
				return;
			}

			if (objh<10)
			{
				return;
			}

			SliderBtnLen=pSliderBar->SliderButtonLength;


			if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=objx+(px_int)((objw-pSliderBar->SliderButtonLength)*(1.0f)*relValue/Range);


						rect.x=(px_float)Sx;
						rect.y=(px_float)objy;
						rect.width=(px_float)pSliderBar->SliderButtonLength;

						rect.height=(px_float)objh;

						if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),rect))
						{
							pSliderBar->btnDownX=x;
							pSliderBar->btnDownY=y;
							pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG;
						}
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						Sx=(px_int)objx+(px_int)((objw-pSliderBar->SliderButtonLength)*(1.0f)*relValue/Range);

						rect.x=(px_float)Sx;
						rect.y=(px_float)objy;
						rect.width=(px_float)pSliderBar->SliderButtonLength;
						rect.height=(px_float)objh;

						if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),rect))
						{
							pSliderBar->btnDownX=x;
							pSliderBar->btnDownY=y;
							pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG;
						}
						else if(x>rect.x)
						{
							pSliderBar->Value+=Range*pSliderBar->SliderButtonLength/((px_int)objw);
							if (pSliderBar->Value>pSliderBar->Max)
							{
								pSliderBar->Value=pSliderBar->Max;
							}
							if (pSliderBar->lastValue != pSliderBar->Value)
							{
								pSliderBar->lastValue = pSliderBar->Value;
								PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
							}
						}
						else
						{
							pSliderBar->Value-=Range*pSliderBar->SliderButtonLength/((px_int)objw);
							if (pSliderBar->Value<0)
							{
								pSliderBar->Value=0;
							}
							if (pSliderBar->lastValue != pSliderBar->Value)
							{
								pSliderBar->lastValue = pSliderBar->Value;
								PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
							}
						}

					}
					break;
				}


			}

			if (pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_VERTICAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=(px_int)objx;
						Sy=(px_int)objy+((px_int)objh-pSliderBar->SliderButtonLength)*relValue/Range;

						rect.x=(px_float)Sx;
						rect.y=(px_float)Sy;
						rect.width=(px_float)objh;
						rect.height=(px_float)pSliderBar->SliderButtonLength;

						if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),rect))
						{
							pSliderBar->btnDownX=x;
							pSliderBar->btnDownY=y;
							pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG;
						}
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						Sy=(px_int)objy+(px_int)((objh-pSliderBar->SliderButtonLength)*(1.0f)*relValue/Range);


						rect.x=(px_float)objx+2;
						rect.y=(px_float)Sy;

						rect.width=(px_float)objw;
						rect.height=(px_float)pSliderBar->SliderButtonLength;

						if (PX_isPointInRect(PX_POINT((px_float)x,(px_float)y,0),rect))
						{
							pSliderBar->btnDownX=x;
							pSliderBar->btnDownY=y;
							pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG;
						}
						else if(y>rect.y)
						{
							pSliderBar->Value+=Range*pSliderBar->SliderButtonLength/((px_int)objh);
							if (pSliderBar->Value>pSliderBar->Max)
							{
								pSliderBar->Value=pSliderBar->Max;
							}
							if (pSliderBar->lastValue != pSliderBar->Value)
							{
								pSliderBar->lastValue = pSliderBar->Value;
								PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
							}
						}
						else
						{
							pSliderBar->Value-=Range*pSliderBar->SliderButtonLength/((px_int)objh);
							if (pSliderBar->Value<pSliderBar->Min)
							{
								pSliderBar->Value=pSliderBar->Min;
							}
							if (pSliderBar->lastValue != pSliderBar->Value)
							{
								pSliderBar->lastValue = pSliderBar->Value;
								PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
							}
						}
					}
					break;
				}

			}
		}
	}
}

px_void PX_Object_SliderBarOnCursorNormal(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pObject);

	if (pSliderBar)
	{
		pSliderBar->status=PX_OBJECT_SLIDERBAR_STATUS_NORMAL;
	}
}

px_void PX_Object_SliderBarOnCursorDrag(PX_Object *pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pObject);
	px_float x,y;

	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	x=PX_Object_Event_GetCursorX(e);
	y=PX_Object_Event_GetCursorY(e);

	if (pSliderBar)
	{
		if (pSliderBar->status!=PX_OBJECT_SLIDERBAR_STATUS_ONDRAG)
		{
			return;
		}

		if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
		{
			if (x<objx)
			{
				pSliderBar->DargButtonX=0;
				return;
			}
			if (x>objx+objWidth)
			{
				pSliderBar->DargButtonX=objWidth-pSliderBar->SliderButtonLength;
				return;
			}

			pSliderBar->DargButtonX+=(x-pSliderBar->btnDownX);
			pSliderBar->btnDownX=x;
			if (pSliderBar->DargButtonX<0)
			{
				pSliderBar->DargButtonX=0;
			}
			if (pSliderBar->DargButtonX>=objWidth-pSliderBar->SliderButtonLength)
			{
				pSliderBar->DargButtonX=objWidth-pSliderBar->SliderButtonLength;
			}

		}

		if (pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_VERTICAL)
		{
			if (y<objy)
			{
				pSliderBar->DargButtonY=0;
				return;
			}
			if (y>objy+objHeight)
			{
				pSliderBar->DargButtonY=objHeight-pSliderBar->SliderButtonLength;
				return;
			}

			pSliderBar->DargButtonY+=(y-pSliderBar->btnDownY);
			pSliderBar->btnDownY=y;

			if (pSliderBar->DargButtonY<0)
			{
				pSliderBar->DargButtonY=0;
			}
			if (pSliderBar->DargButtonY>=objHeight-pSliderBar->SliderButtonLength)
			{
				pSliderBar->DargButtonY=objHeight-pSliderBar->SliderButtonLength;
			}
		}

	}
}

PX_Object * PX_Object_SliderBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_OBJECT_SLIDERBAR_TYPE Type,PX_OBJECT_SLIDERBAR_STYLE style)
{
	PX_Object *pObject;
	PX_Object_SliderBar *pSliderbar=(PX_Object_SliderBar *)MP_Malloc(mp,sizeof(PX_Object_SliderBar));
	if (pSliderbar==PX_NULL)
	{
		return PX_NULL;
	}
	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)Width,(px_float)Height,0);
	if (pObject==PX_NULL)
	{
		MP_Free(pObject->mp,pSliderbar);
		return PX_NULL;
	}

	pObject->pObject=pSliderbar;
	pObject->Enabled=PX_TRUE;
	pObject->Visible=PX_TRUE;
	pObject->Type=PX_OBJECT_TYPE_SLIDERBAR;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_SliderBarFree;
	pObject->Func_ObjectRender=PX_Object_SliderBarRender;
	pSliderbar->Max=100;
	pSliderbar->Value=0;
	pSliderbar->SliderButtonLength=16;
	pSliderbar->style=style;
	pSliderbar->status=PX_OBJECT_SLIDERBAR_STATUS_NORMAL;
	pSliderbar->Type=Type;
	pSliderbar->color=PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pSliderbar->BackgroundColor=PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	pSliderbar->btnDownX=0;
	pSliderbar->btnDownY=0;
	pSliderbar->DargButtonX=0;
	pSliderbar->DargButtonY=0;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDOWN,PX_Object_SliderBarOnMouseLButtonDown,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORMOVE,PX_Object_SliderBarOnCursorNormal,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORUP,PX_Object_SliderBarOnCursorNormal,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_CURSORDRAG,PX_Object_SliderBarOnCursorDrag,PX_NULL);
	return pObject;
}

PX_Object_SliderBar * PX_Object_GetSliderBar( PX_Object *Object )
{
	if(Object->Type==PX_OBJECT_TYPE_SLIDERBAR)
		return (PX_Object_SliderBar *)Object->pObject;
	else
		return PX_NULL;
}

px_void PX_Object_SliderBarSetValue( PX_Object *pSliderBar,px_int Value )
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		if (Value>SliderBar->Max)
		{
			Value=SliderBar->Max;
		}
		if (Value<SliderBar->Min)
		{
			Value=SliderBar->Min;
		}
		SliderBar->Value=Value;
	}
}

px_void PX_Object_SliderBarSetRange( PX_Object *pSliderBar,px_int Min,px_int Max )
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		SliderBar->Min=Min;
		SliderBar->Max=Max;
	}
}

px_int PX_Object_SliderBarGetMax( PX_Object *pSliderBar )
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		return SliderBar->Max;
	}
	return 0;
}

px_int PX_Object_SliderBarGetValue( PX_Object *pSliderBar )
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		if (SliderBar->Value>SliderBar->Max)
		{
			SliderBar->Value=SliderBar->Max;
		}
		if (SliderBar->Value<SliderBar->Min)
		{
			SliderBar->Value=SliderBar->Min;
		}
		return SliderBar->Value;
	}
	return 0;
}

px_void PX_Object_SliderBarRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	PX_Object_SliderBar *pSliderBar=PX_Object_GetSliderBar(pObject);
	px_int SliderBtnLen;
	px_float Sx,Sy;
	px_int Range;
	px_int relValue;
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (pSliderBar==PX_NULL)
	{
		return;
	}

	if (objWidth<=10)
	{
		return;
	}

	if (objHeight<10)
	{
		return;
	}

	Range=pSliderBar->Max-pSliderBar->Min;
	SliderBtnLen=pSliderBar->SliderButtonLength;

	if (pSliderBar->Value>=pSliderBar->Max)
	{
		pSliderBar->Value=pSliderBar->Max;
	}

	if (pSliderBar->Value<pSliderBar->Min)
	{
		pSliderBar->Value=pSliderBar->Min;
	}


	//Clear
	PX_SurfaceClear(
		psurface,
		(px_int)objx,
		(px_int)objy,
		(px_int)objx+(px_int)objWidth-1,
		(px_int)objy+(px_int)objHeight-1,
		pSliderBar->BackgroundColor
		);




	switch(pSliderBar->status)
	{
	case PX_OBJECT_SLIDERBAR_STATUS_NORMAL:
		{
			if (pObject->OnFocus)
			{
				PX_ObjectClearFocus(pObject);
			}
			if (pSliderBar->Value>pSliderBar->Max)
			{
				pSliderBar->Value=pSliderBar->Max;
			}
			if (pSliderBar->Value<pSliderBar->Min)
			{
				pSliderBar->Value=pSliderBar->Min;
			}
			relValue=pSliderBar->Value-pSliderBar->Min;
			if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=(objWidth-pSliderBar->SliderButtonLength)*relValue/Range;
						pSliderBar->DargButtonX=Sx;
						Sx+=objx;
						Sy=objy;
						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)objx,
							(px_int)objy+(px_int)objHeight/2,
							(px_int)objx+(px_int)objWidth-1,
							(px_int)objy+(px_int)objHeight/2,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,(px_int)Sx,(px_int)Sy,(px_int)(Sx+SliderBtnLen-1),(px_int)(Sy+objHeight-1),pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,1,pSliderBar->color);
						Sx=((objWidth-pSliderBar->SliderButtonLength)*(1.0f)*relValue/Range);
						pSliderBar->DargButtonX=Sx;
						Sx+=SliderBtnLen/2;
						//draw slider bar
						PX_GeoDrawRect(psurface,(px_int)(objx+Sx-SliderBtnLen/2+2),(px_int)(objy+2),(px_int)(objx+Sx-SliderBtnLen/2+SliderBtnLen-3),(px_int)(objy+objHeight-3),pSliderBar->color);
					}
					break;
				}


			}

			if (pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_VERTICAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=objx;
						Sy=(objHeight-pSliderBar->SliderButtonLength)*relValue/Range;
						pSliderBar->DargButtonY=Sy;
						Sy+=(px_int)objy;
						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)objx+(px_int)objWidth/2,
							(px_int)objy,
							(px_int)objx+(px_int)objWidth/2,
							(px_int)objy+(px_int)objHeight-1,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,(px_int)Sx,(px_int)Sy,(px_int)(Sx+objWidth-1),(px_int)(Sy+SliderBtnLen-1),pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,1,pSliderBar->color);
						Sy=((objHeight-pSliderBar->SliderButtonLength)*(1.0f)*relValue/Range);
						pSliderBar->DargButtonY=Sy;
						Sy+=SliderBtnLen/2;
						//draw slider bar
						PX_GeoDrawRect(psurface,(px_int)objx+2,(px_int)(objy+Sy-SliderBtnLen/2+2),(px_int)(objx+objWidth-1-2),(px_int)(objy+Sy+SliderBtnLen/2-3),pSliderBar->color);
					}
					break;
				}

			}
		}
		break;
	case PX_OBJECT_SLIDERBAR_STATUS_ONDRAG:
		{
			if (!pObject->OnFocus)
			{
				PX_ObjectSetFocus(pObject);
			}
			if(pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						Sx=pSliderBar->DargButtonX;
						Sy=objy;

						pSliderBar->Value=(px_int)(pSliderBar->Min+Range*(Sx)/(objWidth-pSliderBar->SliderButtonLength));
						if (pSliderBar->Value>pSliderBar->Max)
						{
							pSliderBar->Value=pSliderBar->Max;
						}
						if (pSliderBar->Value<pSliderBar->Min)
						{
							pSliderBar->Value=pSliderBar->Min;
						}
						if (pSliderBar->lastValue != pSliderBar->Value)
						{
							pSliderBar->lastValue = pSliderBar->Value;
							PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
						}
						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)objx,
							(px_int)objy+(px_int)objHeight/2,
							(px_int)objx+(px_int)objWidth-1,
							(px_int)objy+(px_int)objHeight/2,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,(px_int)(objx+Sx),(px_int)Sy,(px_int)(objx+Sx+SliderBtnLen-1),(px_int)(Sy+objHeight-1),pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,1,pSliderBar->color);

						Sx=pSliderBar->DargButtonX;

						pSliderBar->Value=(px_int)(pSliderBar->Min+Range*(Sx)/(objWidth-pSliderBar->SliderButtonLength));
						if (pSliderBar->Value>pSliderBar->Max)
						{
							pSliderBar->Value=pSliderBar->Max;
						}
						if (pSliderBar->Value<pSliderBar->Min)
						{
							pSliderBar->Value=pSliderBar->Min;
						}
						if (pSliderBar->lastValue != pSliderBar->Value)
						{
							pSliderBar->lastValue = pSliderBar->Value;
							PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
						}
						Sx+=SliderBtnLen/2;
						//draw slider bar
						PX_GeoDrawRect(psurface,(px_int)(objx+Sx-SliderBtnLen/2+2),(px_int)objy+2,(px_int)(objx+Sx+SliderBtnLen/2-3),(px_int)(objy+objHeight-3),pSliderBar->color);
					}
					break;
				}


			}

			if (pSliderBar->Type==PX_OBJECT_SLIDERBAR_TYPE_VERTICAL)
			{
				switch(pSliderBar->style)
				{
				case PX_OBJECT_SLIDERBAR_STYLE_LINER:
					{
						pSliderBar->Value=(px_int)(pSliderBar->Min+Range*(pSliderBar->DargButtonY)/(objHeight-pSliderBar->SliderButtonLength));
						if (pSliderBar->Value>pSliderBar->Max)
						{
							pSliderBar->Value=pSliderBar->Max;
						}
						if (pSliderBar->Value<pSliderBar->Min)
						{
							pSliderBar->Value=pSliderBar->Min;
						}
						if (pSliderBar->lastValue != pSliderBar->Value)
						{
							pSliderBar->lastValue = pSliderBar->Value;
							PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
						}
						Sy=pSliderBar->DargButtonY+objy;
						Sx=objx;



						//Draw Line
						PX_GeoDrawRect(
							psurface,
							(px_int)objx+(px_int)objWidth/2,
							(px_int)objy,
							(px_int)objx+(px_int)objWidth/2,
							(px_int)objy+(px_int)objHeight-1,
							pSliderBar->color
							);
						//Draw Slider button
						PX_GeoDrawRect(psurface,(px_int)Sx,(px_int)(Sy),(px_int)(Sx+objWidth-1),(px_int)(Sy+SliderBtnLen-1),pSliderBar->color);
					}
					break;
				case PX_OBJECT_SLIDERBAR_STYLE_BOX:
					{
						//draw border
						PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)objx+(px_int)objWidth-1,(px_int)objy+(px_int)objHeight-1,1,pSliderBar->color);

						pSliderBar->Value=(px_int)(pSliderBar->Min+Range*(pSliderBar->DargButtonY)/(objHeight-pSliderBar->SliderButtonLength));
						if (pSliderBar->Value>pSliderBar->Max)
						{
							pSliderBar->Value=pSliderBar->Max;
						}
						if (pSliderBar->Value<pSliderBar->Min)
						{
							pSliderBar->Value=pSliderBar->Min;
						}
						if (pSliderBar->lastValue != pSliderBar->Value)
						{
							pSliderBar->lastValue = pSliderBar->Value;
							PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
						}
						Sy=pSliderBar->DargButtonY;
						Sx=objx;
						Sy+=SliderBtnLen/2;


						PX_GeoDrawRect(psurface,(px_int)objx+2,(px_int)(objy+Sy-SliderBtnLen/2+2),(px_int)(objx+objWidth-1-2),(px_int)(objy+Sy+SliderBtnLen/2-3),pSliderBar->color);
					}
					break;
				}

			}

		}
		break;
	}

	

	if (pSliderBar->Value>=pSliderBar->Max)
	{
		pSliderBar->Value=pSliderBar->Max;
	}

	if (pSliderBar->Value<0)
	{
		pSliderBar->Value=0;
	}

}

px_void PX_Object_SliderBarSetBackgroundColor(PX_Object *pSliderBar,px_color color)
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		SliderBar->BackgroundColor=color;
	}
}

px_void PX_Object_SliderBarFree( PX_Object *pSliderBar )
{

}

px_void PX_Object_SliderBarSetColor(PX_Object *pSliderBar,px_color color)
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		SliderBar->color=color;
	}
}

px_void PX_Object_SliderBarSetSliderButtonLength(PX_Object *pSliderBar,px_int length)
{
	PX_Object_SliderBar *SliderBar=PX_Object_GetSliderBar(pSliderBar);
	if (SliderBar!=PX_NULL)
	{
		SliderBar->SliderButtonLength=length;
	}
}

