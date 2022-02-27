#include "PX_Object_Widget.h"


px_void PX_Object_WidgetFree(PX_Object *pObject)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		PX_ObjectDelete(pWidget->root);
		PX_TextureFree(&pWidget->renderTarget);
	}
}

px_void  PX_Object_WidgetLinkChild(PX_Object *parent,PX_Object *child)
{
	PX_ObjectAddClild(PX_Object_WidgetGetRoot(parent),child);
}

px_void PX_Object_WidgetShow(PX_Object *pObject)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		pObject->Visible=PX_TRUE;
		PX_ObjectSetFocus(pObject);
	}
}

px_void PX_Object_WidgetHide(PX_Object *pObject)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		pObject->Visible=PX_FALSE;
		PX_ObjectReleaseFocus(pObject);
	}
}

px_void PX_Object_WidgetShowHideCloseButton(PX_Object *pObject,px_bool show)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		pWidget->btn_close->Visible=show;
	}
}

static px_void PX_Object_WidgetOnButtonClose(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object *pWidgetObject=(PX_Object *)ptr;
	PX_ObjectExecuteEvent(pWidgetObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_CLOSE));
	PX_Object_WidgetHide(pWidgetObject);
	
}

px_void PX_Object_Widget_EventDispatcher(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event oe;
	PX_Object_Widget *pwidget=PX_Object_GetWidget(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);
	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;


	switch(e.Event)
	{
	case PX_OBJECT_EVENT_CURSORDOWN:
		{
			px_float x,y;

			if (PX_ObjectIsCursorInRegion(pObject,e))
			{
				PX_ObjectSetFocus(pObject);
				x=PX_Object_Event_GetCursorX(e);
				y=PX_Object_Event_GetCursorY(e);
				if (!pwidget->bDraging)
				{
					if (x>objx&&x<objx+pObject->Width&&y>objy&&y<=objy+PX_OBJECT_WIDGET_BAR_SIZE)
					{
						if (pwidget->bmoveable)
						{
							pwidget->bDraging=PX_TRUE;
							pwidget->lastDragPosition.x=x;
							pwidget->lastDragPosition.y=y;
							pwidget->lastDragPosition.z=0;
						}
						
					}
				}
				pwidget->bevent_update=PX_TRUE;
			}
			else
			{
				if (!pwidget->bmodel)
				{
					PX_ObjectClearFocus(pObject);
				}
				
			}
		}
		break;
	case PX_OBJECT_EVENT_CURSORMOVE:
	case PX_OBJECT_EVENT_CURSORUP:
		{
			pwidget->bDraging=PX_FALSE;
		}
		break;
	case PX_OBJECT_EVENT_CURSORDRAG:
		{
			if (pwidget->bDraging)
			{
				px_float x,y;
				x=PX_Object_Event_GetCursorX(e);
				y=PX_Object_Event_GetCursorY(e);

				pObject->x+=x-pwidget->lastDragPosition.x;
				pObject->y+=y-pwidget->lastDragPosition.y;
				pwidget->lastDragPosition.x=x;
				pwidget->lastDragPosition.y=y;
			}
		}
		break;
	default:
		{
			
		}
		break;
	}

	oe=PX_Object_Event_CursorOffset(e,PX_POINT(-objx,-objy-PX_OBJECT_WIDGET_BAR_SIZE,0));
	PX_ObjectPostEvent(pwidget->root,oe);
}

px_void PX_Object_WidgetRender(px_surface *psurface, PX_Object *pObject,px_uint elapsed)
{
	PX_Object_Widget *pwidget=PX_Object_GetWidget(pObject);
	px_float objx,objy,objWidth,objHeight;
	px_float inheritX,inheritY;

	PX_ObjectGetInheritXY(pObject,&inheritX,&inheritY);

	objx=(pObject->x+inheritX);
	objy=(pObject->y+inheritY);
	objWidth=pObject->Width;
	objHeight=pObject->Height;

	if (!pObject->Visible)
	{
		return;
	}

	if (pObject->Width!=(px_int)(pwidget->renderTarget.width)||pObject->Height!=(px_int)(pwidget->renderTarget.height+PX_OBJECT_WIDGET_BAR_SIZE))
	{
		px_texture *old=&pwidget->renderTarget;
		if(!PX_TextureCreate(pObject->mp,&pwidget->renderTarget,(px_int)pObject->Width,(px_int)pObject->Height+PX_OBJECT_WIDGET_BAR_SIZE))return;
		PX_TextureFree(old);
	}

	do 
	{
		if (pwidget->event_optimization)
		{
			if (!pwidget->bevent_update)
			{
				break;
			}
		}

		pwidget->bevent_update=PX_FALSE;
		PX_SurfaceClear(&pwidget->renderTarget,0,0,pwidget->renderTarget.width-1,pwidget->renderTarget.height-1,pwidget->backgroundcolor);
		PX_ObjectRender(&pwidget->renderTarget,pwidget->root,elapsed);
		PX_SurfaceRender(psurface,&pwidget->renderTarget,(px_int)objx,(px_int)objy+PX_OBJECT_WIDGET_BAR_SIZE,PX_ALIGN_LEFTTOP,PX_NULL);
		if (pwidget->showShader)
		{
			PX_GeoDrawRect(psurface, (px_int)(objx + objWidth), (px_int)objy + 6, (px_int)(objx + objWidth + 6), (px_int)(objy + objHeight + 6), PX_COLOR(128, 0, 0, 0));
			PX_GeoDrawRect(psurface, (px_int)(objx + 6), (px_int)(objy + objHeight), (px_int)(objx + objWidth - 1), (px_int)(objy + objHeight + 6), PX_COLOR(128, 0, 0, 0));
		}
		
	} while (0);


	PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+pObject->Width-1),(px_int)objy+PX_OBJECT_WIDGET_BAR_SIZE-1,pwidget->barColor);

	if (pObject->OnFocus)
		PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+pObject->Width-1),(px_int)(objy+pObject->Height-1),1,pwidget->focusColor);
	else
		PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+pObject->Width-1),(px_int)(objy+pObject->Height-1),1,pwidget->borderColor);

	PX_FontModuleDrawText(psurface,pwidget->fontmodule,(px_int)objx+3,(px_int)objy+3,PX_ALIGN_LEFTTOP,pwidget->title,pwidget->fontcolor);
	
}


PX_Object * PX_Object_WidgetCreate(px_memorypool *mp,PX_Object *Parent,int x,int y,int width,int height,const px_char title[],PX_FontModule *fontmodule)
{
	PX_Object *pObject;
	PX_Object_Widget *pWidget=(PX_Object_Widget *)MP_Malloc(mp,sizeof(PX_Object_Widget));

	if(width<=0) width=1;
	if(height<=PX_OBJECT_WIDGET_BAR_SIZE)height=PX_OBJECT_WIDGET_BAR_SIZE+1;

	if (pWidget==PX_NULL)
	{
		return PX_NULL;
	}
	PX_memset(pWidget,0,sizeof(PX_Object_Widget));

	pObject=PX_ObjectCreate(mp,Parent,(px_float)x,(px_float)y,0,(px_float)width,(px_float)height,0);

	if (pObject==PX_NULL)
	{

		return PX_NULL;
	}

	pObject->pObject=pWidget;
	pObject->Type=PX_OBJECT_TYPE_WIDGET;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=PX_Object_WidgetFree;
	pObject->Func_ObjectRender=PX_Object_WidgetRender;
	pObject->OnLostFocusReleaseEvent=PX_FALSE;

	if (PX_strlen(title)<sizeof(pWidget->title)-1)
	{
		PX_strcpy(pWidget->title,title,sizeof(pWidget->title));
	}

	if(!PX_TextureCreate(mp,&pWidget->renderTarget,width,height-PX_OBJECT_WIDGET_BAR_SIZE))return PX_FALSE;

	pWidget->root=PX_ObjectCreate(mp,PX_NULL,0,0,0,0,0,0);
	pWidget->btn_close=PX_Object_PushButtonCreate(mp,pObject,width-PX_OBJECT_WIDGET_BAR_SIZE,3,PX_OBJECT_WIDGET_BAR_SIZE-6,PX_OBJECT_WIDGET_BAR_SIZE-6,"-",PX_NULL);

	if(!pWidget->btn_close) goto _ERROR;
	pWidget->fontmodule=fontmodule;

	pWidget->backgroundcolor=PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	pWidget->barColor=PX_COLOR(255,128,128,128);
	pWidget->borderColor=PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	pWidget->focusColor=PX_COLOR_WHITE;
	pWidget->bevent_update=PX_TRUE;
	pWidget->bmoveable=PX_TRUE;
	pWidget->fontcolor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pWidget->showShader = PX_TRUE;
	PX_ObjectRegisterEvent(pWidget->btn_close,PX_OBJECT_EVENT_EXECUTE,PX_Object_WidgetOnButtonClose,pObject);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_ANY,PX_Object_Widget_EventDispatcher,PX_NULL);

	pObject->Func_ObjectLinkChild=PX_Object_WidgetLinkChild;
	return pObject;
_ERROR:
	if(pWidget)
		MP_Free(mp,pWidget);
	if(pObject)
		PX_ObjectDelete(pObject);
	return PX_FALSE;
}

PX_Object_Widget * PX_Object_GetWidget(PX_Object *Object)
{
	if (Object->Type==PX_OBJECT_TYPE_WIDGET)
	{
		return (PX_Object_Widget *)Object->pObject;
	}
	return PX_NULL;
}

px_void PX_Object_WidgetSetModel(PX_Object *pObject,px_bool model)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		pWidget->bmodel=model;
	}
}

PX_Object * PX_Object_WidgetGetRoot(PX_Object *pObject)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		return pWidget->root;
	}
	return PX_NULL;
}

px_int PX_Object_WidgetGetRenderTargetHeight(PX_Object *pObject)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		return pWidget->renderTarget.height;
	}
	return 0;
}

px_int PX_Object_WidgetGetRenderTargetWidth(PX_Object *pObject)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		return pWidget->renderTarget.width;
	}
	return 0;
}

px_void PX_Object_WidgetSetBorderColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		pWidget->borderColor=clr;
	}
}

px_void PX_Object_WidgetSetBarColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		pWidget->barColor=clr;
	}
}

px_void PX_Object_WidgetSetBackgroundColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		pWidget->backgroundcolor=clr;
	}
}

px_void PX_Object_WidgetSetFocusColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		pWidget->focusColor=clr;
	}
}

px_void PX_Object_WidgetSetFontColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		pWidget->fontcolor=clr;
	}
}

px_void PX_Object_WidgetSetTitle(PX_Object *pObject,const px_char title[])
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		PX_strcpy(pWidget->title,title,sizeof(pWidget->title));
	}
}
