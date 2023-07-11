#include "PX_Object_Canvas.h"

px_void PX_Object_CanvasOnRender(px_surface* psurface, PX_Object* pObject, px_dword elapsed)
{
	px_float x, y, w, h;
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);

	PX_SurfaceRender(psurface, &pdesc->pCanvasVM->view_surface, (px_int)x, (px_int)y, PX_ALIGN_LEFTTOP,0);
	PX_GeoDrawBorder(psurface, (px_int)x, (px_int)y, (px_int)(x + w), (px_int)(y + h), 1, PX_COLOR_BLACK);
	PX_SurfaceSetLimit(psurface, (px_int)x, (px_int)y, (px_int)(x + w), (px_int)(h + y));
	PX_GeoDrawCircle(psurface, (px_int)(x+pdesc->reg_x), (px_int)(y+pdesc->reg_y),(px_int)(pdesc->pCanvasVM->reg_size * pdesc->pCanvasVM->scale), 1, PX_COLOR(255, 0, 0, 0));
	PX_SurfaceUnlimit(psurface);
}


px_void PX_Object_CanvasUpdateSliderBar(PX_Object* pObject)
{
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	px_float w = pdesc->pCanvasVM->cache_surface.width*1.f;
	px_float h = pdesc->pCanvasVM->cache_surface.height * 1.f;
	px_float vw = pdesc->pCanvasVM->view_surface.width / pdesc->pCanvasVM->scale;
	px_float vh = pdesc->pCanvasVM->view_surface.height / pdesc->pCanvasVM->scale;
	px_float range_min_x = 0;
	px_float range_max_x = pdesc->pCanvasVM->cache_surface.width * 1.f;
	px_float range_min_y =  0;
	px_float range_max_y = pdesc->pCanvasVM->cache_surface.height * 1.f;
	px_float len_x = vw / w * pdesc->pHSliderBar->Width;
	px_float len_y = vh / h * pdesc->pVSliderBar->Height;
	px_float valuex = pdesc->pCanvasVM->view_x;
	px_float valuey = pdesc->pCanvasVM->view_y;

	if (len_x >= pdesc->pHSliderBar->Width/1.5f)
	{
		len_x = pdesc->pHSliderBar->Width/1.5f;
	}
	if (len_y >= pdesc->pVSliderBar->Height/1.5f)
	{
		len_y = pdesc->pVSliderBar->Height/1.5f;
	}


	PX_Object_SliderBarSetRange(pdesc->pHSliderBar, (px_int)range_min_x, (px_int)range_max_x);
	PX_Object_SliderBarSetSliderButtonLength(pdesc->pHSliderBar, (px_int)len_x);
	PX_Object_SliderBarSetValue(pdesc->pHSliderBar, (px_int)valuex);
	
	
	PX_Object_SliderBarSetRange(pdesc->pVSliderBar, (px_int)range_min_y, (px_int)range_max_y);
	PX_Object_SliderBarSetSliderButtonLength(pdesc->pVSliderBar, (px_int)len_y);
	PX_Object_SliderBarSetValue(pdesc->pVSliderBar, (px_int)valuey);
	
}


px_void PX_Object_CanvasOnCursorWheeling(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	px_float cx, cy, cz;
	px_float x, y,w,h;

	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);
	if (PX_ObjectIsCursorInRegion(pObject,e))
	{
		px_float scale,new_v_x,new_v_y;
		px_point2D p2d;
		cx = PX_Object_Event_GetCursorX(e)-x;
		cy = PX_Object_Event_GetCursorY(e)-y;
		cz = PX_Object_Event_GetCursorZ(e);
		
		if (cz > 0)
		{
			scale = 1.1f;
			new_v_x = cx - cx / scale + pdesc->pCanvasVM->view_surface.width / scale / 2;
			new_v_y = cy - cy / scale + pdesc->pCanvasVM->view_surface.height / scale / 2;
			p2d = PX_CanvasVMViewPositionToCanvasPostion(pdesc->pCanvasVM, PX_POINT2D(new_v_x, new_v_y));
			PX_CanvasVMSetViewPosition(pdesc->pCanvasVM,p2d.x, p2d.y);
		}
		else
		{
			scale = 0.9f;
		}
		

		PX_CanvasVMSetViewScale(pdesc->pCanvasVM,pdesc->pCanvasVM->scale * scale);
		PX_Object_CanvasUpdateSliderBar(pObject);
	}
	
}

px_void PX_Object_CanvasOnHSliderBarChanged(PX_Object* pObject, PX_Object_Event  e, px_void* ptr)
{
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, (PX_Object*)ptr);

	PX_CanvasVMSetViewPosition(pdesc->pCanvasVM, PX_Object_SliderBarGetValue(pObject) * 1.f, pdesc->pCanvasVM->view_y);
	
}

px_void PX_Object_CanvasOnVSliderBarChanged(PX_Object* pObject, PX_Object_Event  e, px_void* ptr)
{
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, (PX_Object*)ptr);
	PX_CanvasVMSetViewPosition(pdesc->pCanvasVM, pdesc->pCanvasVM->view_x, PX_Object_SliderBarGetValue(pObject) * 1.f);
}

px_void PX_Object_CanvasEnterEdit(PX_Object* pObject,px_float x,px_float y,px_float z)
{
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	px_point2D canvaspos;
	
	PX_CanvasVMOnBegin(pdesc->pCanvasVM);
	if (pdesc->pCanvasVM->reg_state==PX_CanvasVM_State_Painting)
	{
		canvaspos = PX_CanvasVMViewPositionToCanvasPostion(pdesc->pCanvasVM, PX_POINT2D(pdesc->reg_x, pdesc->reg_y));
		PX_CanvasVMOnMove(pdesc->pCanvasVM, canvaspos.x, canvaspos.y, z);
	}
	
}


px_void PX_Object_CanvasOnCursorDown(PX_Object* pObject, PX_Object_Event  e, px_void* ptr)
{
	px_float x, y, w, h;
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);

	if (PX_ObjectIsCursorInRegion(pObject,e))
	{
		pdesc->reg_x = PX_Object_Event_GetCursorX(e) - x;
		pdesc->reg_y = PX_Object_Event_GetCursorY(e) - y;
		pdesc->reg_z = 1;
		
		pdesc->pin_x = pdesc->reg_x;
		pdesc->pin_y = pdesc->reg_y;
		pdesc->pin_view_x=pdesc->pCanvasVM->view_x;
        pdesc->pin_view_y=pdesc->pCanvasVM->view_y;
		
		PX_Object_CanvasEnterEdit(pObject,pdesc->reg_x,pdesc->reg_y,pdesc->reg_z);
	}
}


px_void PX_Object_CanvasOnCursorRDown(PX_Object* pObject, PX_Object_Event  e, px_void* ptr)
{
	px_float x, y, w, h;
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);

	PX_CanvasVMMoveBack(pdesc->pCanvasVM);
}


px_void PX_Object_CanvasOnCursorMove(PX_Object* pObject, PX_Object_Event  e, px_void* ptr)
{
	px_float x, y, w, h;
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);
	if (PX_ObjectIsCursorInRegion(pObject, e))
	{
		pdesc->reg_x = PX_Object_Event_GetCursorX(e) - x;
		pdesc->reg_y = PX_Object_Event_GetCursorY(e) - y;
		pdesc->reg_z = 1;
	}

	if (pdesc->pCanvasVM->reg_state==PX_CanvasVM_State_Painting)
	{
		PX_CanvasVMOnEnd(pdesc->pCanvasVM);
		PX_CanvasVMRepaintAllCache(pdesc->pCanvasVM);
	}

}

px_void PX_Object_CanvasOnCursorDrag(PX_Object* pObject, PX_Object_Event  e, px_void* ptr)
{
	px_float x, y, w, h;
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	//px_surface* prender = PX_Object_CanvasGetCurrentEditingSurface(pObject);
	
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);
	if (PX_ObjectIsCursorInRegion(pObject, e))
	{

		pdesc->reg_x = PX_Object_Event_GetCursorX(e) - x;
		pdesc->reg_y = PX_Object_Event_GetCursorY(e) - y;
		pdesc->reg_z = 1;
		
		if (pdesc->pCanvasVM->reg_state==PX_CanvasVM_State_Painting)
		{
			
			if (pdesc->pCanvasVM->reg_tool==0)
			{
				px_float newx,newy;
				//moving
				newx =pdesc->pin_view_x- (pdesc->reg_x-pdesc->pin_x)/pdesc->pCanvasVM->scale;
				newy = pdesc->pin_view_y - (pdesc->reg_y - pdesc->pin_y) /pdesc->pCanvasVM->scale;

				PX_CanvasVMSetViewPosition(pdesc->pCanvasVM, newx, newy);
				PX_Object_CanvasUpdateSliderBar(pObject);
			}
			else
			{
				//editing
				px_point2D canvaspos;
				canvaspos = PX_CanvasVMViewPositionToCanvasPostion(pdesc->pCanvasVM, PX_POINT2D(pdesc->reg_x, pdesc->reg_y));
				PX_CanvasVMOnMove(pdesc->pCanvasVM, canvaspos.x, canvaspos.y, pdesc->reg_z);
				PX_CanvasVMRepaintCache(pdesc->pCanvasVM);
				PX_CanvasVMRepaintCacheToView(pdesc->pCanvasVM);
			}

			
		}
	}
	else
	{
		return;
	}

	
	
}

px_void PX_Object_CanvasOnCursorUp(PX_Object* pObject, PX_Object_Event  e, px_void* ptr)
{
	px_float x, y, w, h;
	PX_Object_Canvas* pdesc = PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);

	if (PX_ObjectIsCursorInRegion(pObject, e))
	{
		pdesc->reg_x = PX_Object_Event_GetCursorX(e) - x;
		pdesc->reg_y = PX_Object_Event_GetCursorY(e) - y;
	}

	if (pdesc->pCanvasVM->reg_state == PX_CanvasVM_State_Painting)
	{
		PX_CanvasVMOnEnd(pdesc->pCanvasVM);
		PX_CanvasVMRepaintAllCache(pdesc->pCanvasVM);

	}
	
}



PX_Object* PX_Object_CanvasCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y,  PX_CanvasVM* pCanvasVM)
{
	PX_Object_Canvas desc = { 0 }, * pdesc;
	PX_Object* pObject;


	pObject = PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)pCanvasVM->view_surface.width+24, (px_float)pCanvasVM->view_surface.height+24, 0, PX_OBJECT_TYPE_CANVAS, 0, PX_Object_CanvasOnRender, PX_NULL, &desc, sizeof(desc));
	pdesc = PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	pdesc->mp = mp;
	pdesc->pCanvasVM = pCanvasVM;

	pdesc->pHSliderBar = PX_Object_SliderBarCreate(mp, pObject, 0, pCanvasVM->view_surface.height, pCanvasVM->view_surface.width, 24, PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);
	pdesc->pVSliderBar = PX_Object_SliderBarCreate(mp, pObject, pCanvasVM->view_surface.width, 0, 24, pCanvasVM->view_surface.height, PX_OBJECT_SLIDERBAR_TYPE_VERTICAL, PX_OBJECT_SLIDERBAR_STYLE_BOX);

	PX_Object_SliderBarSetBackgroundColor(pdesc->pHSliderBar, PX_COLOR_WHITE);
	PX_Object_SliderBarSetColor(pdesc->pHSliderBar, PX_COLOR_BLACK);
	
	PX_Object_SliderBarSetBackgroundColor(pdesc->pVSliderBar, PX_COLOR_WHITE);
	PX_Object_SliderBarSetColor(pdesc->pVSliderBar, PX_COLOR_BLACK);

	PX_Object_CanvasUpdateSliderBar(pObject);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORWHEEL, PX_Object_CanvasOnCursorWheeling, 0);

	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_CanvasOnCursorDown, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORRDOWN, PX_Object_CanvasOnCursorRDown, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_CanvasOnCursorMove, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_CanvasOnCursorDrag, 0);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMUP, PX_Object_CanvasOnCursorUp, 0);
	
	PX_ObjectRegisterEvent(pdesc->pHSliderBar, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_CanvasOnHSliderBarChanged, pObject);
	PX_ObjectRegisterEvent(pdesc->pVSliderBar, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_CanvasOnVSliderBarChanged, pObject);


	return pObject;
}

PX_Object_Canvas* PX_Object_GetCanvas(PX_Object* pObject)
{
	if (pObject->Type==PX_OBJECT_TYPE_CANVAS)
	{
		return PX_ObjectGetDesc(PX_Object_Canvas, pObject);
	}
	return PX_NULL;
}


