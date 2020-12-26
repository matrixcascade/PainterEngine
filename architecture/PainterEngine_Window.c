#include "PainterEngine_Window.h"
px_void PX_WindowOnButtonClose(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Window *pwindow=(PX_Window *)ptr;
	pwindow->visible=PX_FALSE;
}

px_bool PX_WindowInitialize(PX_Runtime *runtime,px_memorypool *mp,PX_Window *pwindow,px_int x,int y,px_int width,px_int height,const px_char title[],PX_FontModule *fontmodule)
{
	PX_memset(pwindow,0,sizeof(PX_Window));
	pwindow->pruntime=runtime;
	pwindow->fontmodule=fontmodule;
	pwindow->x=(px_float)x;
	pwindow->y=(px_float)y;
	pwindow->width=width;
	pwindow->height=height;
	if (PX_strlen(title)<sizeof(pwindow->title)-1)
	{
		PX_strcpy(pwindow->title,title,sizeof(pwindow->title));
	}
	
	if(!PX_TextureCreate(mp,&pwindow->renderTarget,width+PX_WINDOW_BAR_SIZE,height+PX_WINDOW_BAR_SIZE*2))return PX_FALSE;
	
	pwindow->Area=PX_Object_ScrollAreaCreate(mp,PX_NULL,0,PX_WINDOW_BAR_SIZE,height,width);
	if(!pwindow->Area) goto _ERROR;
	PX_Object_ScrollAreaSetBorder(pwindow->Area,PX_FALSE);
	pwindow->HScroll=PX_Object_SliderBarCreate(mp,PX_NULL,0,0,PX_WINDOW_BAR_SIZE,PX_WINDOW_BAR_SIZE,PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	if(!pwindow->HScroll) goto _ERROR;
	pwindow->VScroll=PX_Object_SliderBarCreate(mp,PX_NULL,0,0,1,1,PX_OBJECT_SLIDERBAR_TYPE_VERTICAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	if(!pwindow->VScroll) goto _ERROR;

	pwindow->btn_close=PX_Object_PushButtonCreate(mp,PX_NULL,0,0,PX_WINDOW_BAR_SIZE,PX_WINDOW_BAR_SIZE,"-",PX_NULL,PX_COLOR(255,0,0,0));
	if(!pwindow->btn_close) goto _ERROR;

	pwindow->btn_close->x=(px_float)width;
	pwindow->btn_close->y=0;

	pwindow->HScroll->Visible=PX_FALSE;
	pwindow->VScroll->Visible=PX_FALSE;
	pwindow->onFocus=PX_FALSE;

	pwindow->HScroll->x=0;
	pwindow->HScroll->y=(px_float)pwindow->height;
	pwindow->HScroll->Width=(px_float)pwindow->width;

	pwindow->VScroll->x=(px_float)pwindow->width;
	pwindow->VScroll->y=PX_WINDOW_BAR_SIZE;
	pwindow->VScroll->Height=(px_float)pwindow->height;

	pwindow->visible=PX_TRUE;

	PX_ObjectRegisterEvent(pwindow->btn_close,PX_OBJECT_EVENT_EXECUTE,PX_WindowOnButtonClose,pwindow);

	return PX_TRUE;
_ERROR:
	if(pwindow->Area)PX_ObjectDelete(pwindow->Area);
	if(pwindow->HScroll)PX_ObjectDelete(pwindow->HScroll);
	if(pwindow->VScroll)PX_ObjectDelete(pwindow->VScroll);
	return PX_FALSE;
}

px_void PX_WindowPostEvent(PX_Window *pwindow,PX_Object_Event e)
{
	PX_Object_Event oe;
	if (!pwindow->visible)
	{
		return;
	}
	oe=PX_Object_Event_CursorOffset(e,PX_POINT(-(px_float)pwindow->x,-(px_float)pwindow->y,0));
	switch(oe.Event)
	{
	case PX_OBJECT_EVENT_CURSORDOWN:
		{
			px_float x,y;
			x=PX_Object_Event_GetCursorX(oe);
			y=PX_Object_Event_GetCursorY(oe);
			if (!pwindow->bDraging)
			{
				if (x>0&&x<pwindow->width&&y>0&&y<=PX_WINDOW_BAR_SIZE)
				{
					pwindow->bDraging=PX_TRUE;
					pwindow->lastDragPosition.x=x;
					pwindow->lastDragPosition.y=y;
					pwindow->lastDragPosition.z=0;
				}
			}
		}
		break;
	case PX_OBJECT_EVENT_CURSORMOVE:
	case PX_OBJECT_EVENT_CURSORUP:
		{
			pwindow->bDraging=PX_FALSE;
		}
		break;
	case PX_OBJECT_EVENT_CURSORDRAG:
		{
			if (pwindow->bDraging)
			{
				px_float x,y;
				x=PX_Object_Event_GetCursorX(oe);
				y=PX_Object_Event_GetCursorY(oe);

				pwindow->x+=x-pwindow->lastDragPosition.x;
				pwindow->y+=y-pwindow->lastDragPosition.y;
			}
		}
		break;
	}


	PX_ObjectPostEvent(pwindow->HScroll,oe);
	PX_ObjectPostEvent(pwindow->VScroll,oe);
	PX_ObjectPostEvent(pwindow->Area,oe);
	PX_ObjectPostEvent(pwindow->btn_close,oe);
}

px_void PX_WindowUpdate(PX_Window *pwindow,px_dword elpased)
{
	if (!pwindow->visible)
	{
		return;
	}

	if (pwindow->width!=(pwindow->renderTarget.width-PX_WINDOW_BAR_SIZE)||pwindow->height!=(pwindow->renderTarget.height-2*PX_WINDOW_BAR_SIZE))
	{
		px_texture *old=&pwindow->renderTarget;
		if(!PX_TextureCreate(pwindow->mp,&pwindow->renderTarget,pwindow->width+PX_WINDOW_BAR_SIZE,pwindow->height+PX_WINDOW_BAR_SIZE*2))return;
		PX_TextureFree(old);

		pwindow->HScroll->Visible=PX_FALSE;
		pwindow->VScroll->Visible=PX_FALSE;

		pwindow->HScroll->x=0;
		pwindow->HScroll->y=(px_float)pwindow->height;
		pwindow->HScroll->Width=(px_float)pwindow->width;

		pwindow->VScroll->x=(px_float)pwindow->width;
		pwindow->VScroll->y=PX_WINDOW_BAR_SIZE;
		pwindow->HScroll->Height=(px_float)pwindow->height;
	}



}

px_void PX_WindowClose(PX_Window *pwindow)
{
	pwindow->visible=PX_FALSE;
}

px_void PX_WindowRender(px_surface *pSurface,PX_Window *pwindow,px_dword elpased)
{
	if (!pwindow->visible)
	{
		return;
	}
	PX_SurfaceClear(&pwindow->renderTarget,0,0,pwindow->renderTarget.width-1,pwindow->renderTarget.height-1,PX_COLOR(255,255,255,255));
	PX_ObjectRender(&pwindow->renderTarget,pwindow->HScroll,elpased);
	PX_ObjectRender(&pwindow->renderTarget,pwindow->VScroll,elpased);
	PX_ObjectRender(&pwindow->renderTarget,pwindow->Area,elpased);
	PX_GeoDrawRect(&pwindow->renderTarget,0,0,pwindow->renderTarget.width-1,PX_WINDOW_BAR_SIZE,PX_COLOR(255,192,192,192));
	PX_ObjectRender(&pwindow->renderTarget,pwindow->btn_close,elpased);
	PX_GeoDrawBorder(&pwindow->renderTarget,0,0,pwindow->renderTarget.width-1,pwindow->renderTarget.height-1,1,PX_COLOR(255,0,0,0));
	PX_SurfaceRender(pSurface,&pwindow->renderTarget,(px_int)pwindow->x,(px_int)pwindow->y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
	
}

