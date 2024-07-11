#include "PX_Object_Widget.h"


PX_OBJECT_FREE_FUNCTION(PX_Object_WidgetFree)
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
	PX_ObjectAddChild(PX_Object_WidgetGetRoot(parent),child);
}

px_void PX_Object_WidgetShow(PX_Object *pObject)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	if (pWidget)
	{
		pObject->Visible=PX_TRUE;
		if(pWidget->focusWidget)
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

static PX_OBJECT_EVENT_FUNCTION(PX_Object_WidgetOnButtonClose)
{
	PX_Object *pWidgetObject=(PX_Object *)ptr;
	PX_ObjectExecuteEvent(pWidgetObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_CLOSE));
	PX_Object_WidgetHide(pWidgetObject);
	
}

PX_OBJECT_EVENT_FUNCTION(PX_Object_Widget_EventDispatcher)
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
				if (pwidget->focusWidget)
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

PX_OBJECT_RENDER_FUNCTION(PX_Object_WidgetRender)
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
			px_color sd = pwidget->backgroundcolor;
			sd._argb.a = sd._argb.a/2;
			PX_GeoDrawRect(psurface, (px_int)(objx + objWidth), (px_int)objy + 6, (px_int)(objx + objWidth + 6), (px_int)(objy + objHeight + 6), sd);
			PX_GeoDrawRect(psurface, (px_int)(objx + 6), (px_int)(objy + objHeight), (px_int)(objx + objWidth - 1), (px_int)(objy + objHeight + 6), sd);
		}
		
	} while (0);

	
	PX_GeoDrawRect(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+pObject->Width-1),(px_int)objy+PX_OBJECT_WIDGET_BAR_SIZE-1,pwidget->barColor);

	if (pObject->OnFocus)
		PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+pObject->Width-1),(px_int)(objy+pObject->Height-1),1,pwidget->focusColor);
	else
		PX_GeoDrawBorder(psurface,(px_int)objx,(px_int)objy,(px_int)(objx+pObject->Width-1),(px_int)(objy+pObject->Height-1),1,pwidget->borderColor);

	PX_FontModuleDrawText(psurface,pwidget->fontmodule,(px_int)objx+3,(px_int)objy+3,PX_ALIGN_LEFTTOP,pwidget->title,pwidget->fontcolor);
	
}


PX_Object * PX_Object_WidgetAttachObject(PX_Object *pObject,px_int attachIndex,px_int x,px_int y,px_int width,px_int height,const px_char title[],PX_FontModule *fontmodule)
{
	px_memorypool* mp=pObject->mp;
	PX_Object_Widget *pWidget,*pdesc;

	if (width <= 0) width = 1;
	if (height <= PX_OBJECT_WIDGET_BAR_SIZE)height = PX_OBJECT_WIDGET_BAR_SIZE + 1;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_Widget*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_WIDGET, 0, PX_Object_WidgetRender, PX_Object_WidgetFree, PX_NULL, sizeof(PX_Object_Widget));
	PX_ASSERTIF(pdesc == PX_NULL);

	
	pWidget=PX_Object_GetWidget(pObject);
	
	if (pWidget==PX_NULL)
	{
		return PX_NULL;
	}
	pObject->OnLostFocusReleaseEvent=PX_FALSE;

	if (PX_strlen(title)<sizeof(pWidget->title)-1)
	{
		PX_strcpy(pWidget->title,title,sizeof(pWidget->title));
	}

	
	if (!PX_TextureCreate(mp, &pWidget->renderTarget, width, height - PX_OBJECT_WIDGET_BAR_SIZE))return PX_FALSE;

	pWidget->root=PX_ObjectCreate(mp,PX_NULL,0,0,0,0,0,0);
	pWidget->btn_close=PX_Object_PushButtonCreate(mp,pObject,width-PX_OBJECT_WIDGET_BAR_SIZE,3,PX_OBJECT_WIDGET_BAR_SIZE-6,PX_OBJECT_WIDGET_BAR_SIZE-6,"-",PX_NULL);

	if(!pWidget->btn_close) goto _ERROR;
	pWidget->fontmodule=fontmodule;

	pWidget->backgroundcolor=PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR;
	pWidget->barColor=PX_COLOR(255,128,128,128);
	pWidget->borderColor=PX_OBJECT_UI_DEFAULT_BORDERCOLOR;
	pWidget->focusColor=PX_COLOR_FORCEBORDERCOLOR;
	pWidget->bevent_update=PX_TRUE;
	pWidget->bmoveable=PX_TRUE;
	pWidget->fontcolor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;
	pWidget->showShader = PX_TRUE;
	pWidget->focusWidget = PX_TRUE;
	PX_ObjectRegisterEventEx(pWidget->btn_close,PX_OBJECT_EVENT_EXECUTE, PX_OBJECT_TYPE_WIDGET, PX_Object_WidgetOnButtonClose,pObject);
	PX_ObjectRegisterEventEx(pObject,PX_OBJECT_EVENT_ANY, PX_OBJECT_TYPE_WIDGET, PX_Object_Widget_EventDispatcher,PX_NULL);

	pObject->Func_ObjectLinkChild=PX_Object_WidgetLinkChild;
	return pObject;
_ERROR:
	return PX_NULL;
}

PX_Object* PX_Object_WidgetCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, const px_char title[], PX_FontModule* fontmodule)
{
	PX_Object* pObject;
	pObject = PX_ObjectCreate(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)width, (px_float)height, 0);
	if (pObject == PX_NULL)
	{
		return PX_NULL;
	}
	if (!PX_Object_WidgetAttachObject(pObject, 0, x, y, width, height, title, fontmodule))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}
PX_Object_Widget * PX_Object_GetWidget(PX_Object *pObject)
{
	PX_Object_Widget *pdesc=(PX_Object_Widget *)PX_ObjectGetDescByType(pObject,PX_OBJECT_TYPE_WIDGET);
	PX_ASSERTIF(pdesc==PX_NULL);
	return pdesc;
}

px_void PX_Object_WidgetSetModel(PX_Object *pObject,px_bool model)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	
	pWidget->bmodel=model;
}

PX_Object * PX_Object_WidgetGetRoot(PX_Object *pObject)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	
	return pWidget->root;

}

px_int PX_Object_WidgetGetRenderTargetHeight(PX_Object *pObject)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	
	return pWidget->renderTarget.height;
	
	return 0;
}

px_int PX_Object_WidgetGetRenderTargetWidth(PX_Object *pObject)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);

	return pWidget->renderTarget.width;
	
}

px_surface* PX_Object_WidgetGetRenderTarget(PX_Object* pObject)
{
	PX_Object_Widget* pWidget = PX_Object_GetWidget(pObject);

	return &pWidget->renderTarget;

}

px_void PX_Object_WidgetReleaseFocus(PX_Object* pObject)
{
	PX_ObjectClearFocus(pObject);
}

px_bool PX_Object_WidgetIsOnFocus(PX_Object* pObject)
{
	return PX_ObjectIsOnFocus(pObject);
}

px_void PX_Object_WidgetSetBorderColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);
	
	pWidget->borderColor=clr;
	
}


px_void PX_Object_WidgetSetBarColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);

	pWidget->barColor=clr;
	
}

px_void PX_Object_WidgetSetBackgroundColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);

	pWidget->backgroundcolor=clr;
	
}

px_void PX_Object_WidgetSetFocusColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);

	pWidget->focusColor=clr;
	
}

px_void PX_Object_WidgetSetFontColor(PX_Object *pObject,px_color clr)
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);

	pWidget->fontcolor=clr;
	
}

px_void PX_Object_WidgetSetTitle(PX_Object *pObject,const px_char title[])
{
	PX_Object_Widget *pWidget=PX_Object_GetWidget(pObject);

	PX_strcpy(pWidget->title,title,sizeof(pWidget->title));
	
}

px_void PX_Object_WidgetShowShader(PX_Object* pObject, px_bool show)
{
	PX_Object_Widget* pWidget = PX_Object_GetWidget(pObject);

		pWidget->showShader = show;
	
}

px_void PX_Object_WidgetSetFocusWidget(PX_Object* pObject, px_bool bf)
{
	PX_Object_Widget* pWidget = PX_Object_GetWidget(pObject);

	pWidget->focusWidget = bf;

}
