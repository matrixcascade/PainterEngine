#include "PX_Object_ColorPanel.h"

px_bool PX_Object_ColorPanelUpdatePanelTexture(PX_Object* pObject)
{
	PX_Object_ColorPanel* pdesc;
	px_int x,y,d;
	px_float ed,edis;
	pdesc = PX_ObjectGetDesc(PX_Object_ColorPanel, pObject);
	d = (px_int)(pObject->Width > pObject->Height ? pObject->Height : pObject->Width);
	
	PX_TextureFree(&pdesc->panel);
	
	if (!PX_TextureCreate(pdesc->mp, &pdesc->panel, d, d))
	{
		PX_ObjectDelete(pObject);
		return PX_FALSE;
	}

	
	for ( y = 0; y < d; y++)
	{
		for (x = 0; x < d; x++)
		{
			px_float ax=x-d/2.f, ay=y-d/2.f;
			px_float r = PX_sqrt(ax * ax + ay * ay);
			if (r>d/2-18&&r< d / 2-2)
			{
				px_float angle = (px_float)(PX_atan2(ay, ax) / PX_PI * 180);
				px_int H = (px_int)angle;
				px_color_hsv hsv;
				px_color rgb;
				if (H <0 )H += 360;
				if (H > 360)H -= 360;
				hsv.a = 1;
				hsv.H = H*1.f;
				hsv.S = 1;
				hsv.V = 1;
				rgb = PX_ColorHSVToRGB(hsv);
				PX_SurfaceDrawPixel(&pdesc->panel, x, y, rgb);
			}
		}
	}
	PX_GeoDrawRing(&pdesc->panel, d / 2, d / 2, d / 2-2.f , 2, PX_COLOR_BLACK, 0, 360);
	PX_GeoDrawRing(&pdesc->panel, d / 2, d / 2, d / 2-18.f, 2, PX_COLOR_BLACK, 0, 360);
	ed = d / 2 - (d/2 - 18) / 1.41421f+2;
	edis = d - ed - ed;
	for (y=(px_int)ed;y<d-(px_int)ed;y++ )
	{
		for (x = (px_int)ed; x < d - (px_int)ed; x++)
		{
			px_color_hsv hsv=pdesc->color_hsv;
			px_color rgb;
			hsv.a = 1;
			hsv.S = (x-ed+1)/edis;
			hsv.V = 1-(y-ed+1)/edis;
			rgb = PX_ColorHSVToRGB(hsv);
			PX_SurfaceDrawPixel(&pdesc->panel, x, y, rgb);
		}
	}
	PX_GeoDrawBorder(&pdesc->panel, (px_int)ed, (px_int)ed, (px_int)(d - ed) , (px_int)(d - ed),1, PX_COLOR_BLACK);
	return PX_TRUE;
}


px_void PX_Object_ColorPanelRender(px_surface* psurface, PX_Object* pObject, px_uint elapsed)
{
	px_float x, y, w, h,ax=0,ay=0;
	px_int d;
	PX_Object_ColorPanel * pdesc;
	pdesc = PX_ObjectGetDesc(PX_Object_ColorPanel, pObject);
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);
	d = (px_int)(w > h ? h : w);
	if (w>h)
	{
		ax = w / 2 - d / 2;
	}
	else
	{
		ay = h / 2 - d / 2;
	}

	if (d!=pdesc->panel.width)
	{
		if (!PX_Object_ColorPanelUpdatePanelTexture(pObject))
			return;
	}

	//render
	PX_TextureRender(psurface, &pdesc->panel, (px_int)(x + w / 2),(px_int)( y + h / 2), PX_ALIGN_CENTER, 0);

	do 
	{
		px_float hx, hy;
		px_float ed, edis;
		ed = d / 2 - (d / 2 - 18) / 1.41421f + 2;
		edis = d - ed - ed;
		hx = PX_cos_angle(pdesc->color_hsv.H)*(d/2-10);
		hy= PX_sin_angle(pdesc->color_hsv.H)*(d/2-10);
		PX_GeoDrawCircle(psurface, (px_int)(x + w / 2 + hx), (px_int)(y + h / 2 + hy), 6, 3, PX_COLOR_BLACK);
		PX_GeoDrawCircle(psurface, (px_int)(x + w / 2 + hx), (px_int)(y + h / 2 + hy), 6, 1, PX_COLOR_WHITE);

		hx = ed + (pdesc->color_hsv.S) * edis;
		hy = ed + (1-pdesc->color_hsv.V) * edis;
		PX_GeoDrawCircle(psurface, (px_int)(x  +ax+ hx), (px_int)(y +ay+  hy), 6, 3, PX_COLOR_BLACK);
		PX_GeoDrawCircle(psurface, (px_int)(x  +ax+ hx), (px_int)(y +ay+  hy), 6, 1, PX_COLOR_WHITE);


		PX_GeoDrawSolidCircle(psurface, (px_int)(x + ax + hx), (px_int)(y + ay + hy), 5,  pdesc->color_rgb);
	} while (0);
}

px_void PX_Object_ColorPanelOnCursorCancel(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_ColorPanel* pdesc;
	pdesc = PX_ObjectGetDesc(PX_Object_ColorPanel, pObject);
	pdesc->state = PX_Object_ColorPanel_State_Normal;
}

px_void PX_Object_ColorPanelOnCursorDown(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_ColorPanel* pdesc;
	px_float x, y, w, h;
	
	if (PX_ObjectIsCursorInRegion(pObject, e))
	{
		px_float d;
		px_float cx=PX_Object_Event_GetCursorX(e), cy=PX_Object_Event_GetCursorY(e);
		px_float  edis;
		px_float ringx, ringy,r;
		px_float ed;
		pdesc = PX_ObjectGetDesc(PX_Object_ColorPanel, pObject);
		PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);
		d = pdesc->panel.width*1.f;
		cx -= x; cy -= y;
		if (w>h)
		{
			cx -= w / 2 - d / 2;
		}
		else
		{
			cy -= h / 2 - d / 2;
		}

		ringx = cx - d / 2;
		ringy = cy - d / 2;
		r = PX_sqrt(ringx * ringx + ringy * ringy);
		if (r > d / 2 - 18 && r < d / 2 - 2)
		{
			px_float angle = (px_float)(PX_atan2(ringy, ringx) / PX_PI * 180);
			px_int H = (px_int)angle;
			if (H < 0)H += 360;
			if (H > 360)H -= 360;
			pdesc->color_hsv.H = H*1.f;
			pdesc->color_rgb = PX_ColorHSVToRGB(pdesc->color_hsv);
			PX_Object_ColorPanelUpdatePanelTexture(pObject);
			pdesc->state = PX_Object_ColorPanel_State_Ring;
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}
		
		ed = d / 2 - (d / 2 - 18) / 1.41421f + 2;
		 edis = d - ed - ed;
		cx -= ed;
		cy -= ed;
		if (cx>0&&cx<edis&&cy>0&&cy<edis)
		{
			
			pdesc->color_hsv.S = cx / edis;
			pdesc->color_hsv.V = 1 - cy/ edis;
			pdesc->color_rgb = PX_ColorHSVToRGB(pdesc->color_hsv);
			pdesc->state = PX_Object_ColorPanel_State_SV;
			PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
		}

	}
}

px_void PX_Object_ColorPanelOnCursorDrag(PX_Object* pObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object_ColorPanel* pdesc;
	px_float x, y, w, h;
	px_float d;
	px_float cx = PX_Object_Event_GetCursorX(e), cy = PX_Object_Event_GetCursorY(e);
	px_float  edis;
	px_float ringx, ringy, r;
	px_float ed;
	pdesc = PX_ObjectGetDesc(PX_Object_ColorPanel, pObject);
	PX_OBJECT_INHERIT_CODE(pObject, x, y, w, h);
	if (pdesc->state == PX_Object_ColorPanel_State_Normal)
	{
		return;
	}
	
	d = pdesc->panel.width*1.f;
	cx -= x; cy -= y;
	if (w > h)
	{
		cx -= w / 2 - d / 2;
	}
	else
	{
		cy -= h / 2 - d / 2;
	}

	ringx = cx - d / 2;
	ringy = cy - d / 2;
	r = PX_sqrt(ringx * ringx + ringy * ringy);
		
	if(pdesc->state==PX_Object_ColorPanel_State_Ring)
	{
		px_float angle = (px_float)(PX_atan2(ringy, ringx) / PX_PI * 180);
		px_int H = (px_int)angle;
		if (H < 0)H += 360;
		if (H > 360)H -= 360;
		pdesc->color_hsv.H = H*1.f;
		pdesc->color_rgb = PX_ColorHSVToRGB(pdesc->color_hsv);
		PX_Object_ColorPanelUpdatePanelTexture(pObject);
		PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
	}
	else if (pdesc->state == PX_Object_ColorPanel_State_SV)
	{
		ed = d / 2 - (d / 2 - 18) / 1.41421f + 2;
		 edis = d - ed - ed;
		cx -= ed;
		cy -= ed;
		
		pdesc->color_hsv.S = cx / edis;
		if (pdesc->color_hsv.S < 0)
			pdesc->color_hsv.S = 0;
		if (pdesc->color_hsv.S > 1)
			pdesc->color_hsv.S = 1;

		pdesc->color_hsv.V = 1 - cy / edis;

		if (pdesc->color_hsv.V < 0)
			pdesc->color_hsv.V = 0;
		if (pdesc->color_hsv.V > 1)
			pdesc->color_hsv.V = 1;

		pdesc->color_rgb = PX_ColorHSVToRGB(pdesc->color_hsv);
		PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
	}

}

px_color PX_Object_ColorPanelGetColor(PX_Object* pObject)
{
	PX_Object_ColorPanel* pdesc;
	pdesc = PX_ObjectGetDesc(PX_Object_ColorPanel, pObject);
	return pdesc->color_rgb;
}
PX_Object_ColorPanel* PX_Object_GetColorPanel(PX_Object* pObject)
{
	PX_Object_ColorPanel* pdesc;
	pdesc = PX_ObjectGetDesc(PX_Object_ColorPanel, pObject);
	if(pObject->Type== PX_OBJECT_TYPE_COLORPANEL)
		return pdesc;
	return PX_NULL;
}

px_void PX_Object_ColorPanelFree(PX_Object* pObject)
{
	PX_Object_ColorPanel* pdesc = PX_ObjectGetDesc(PX_Object_ColorPanel, pObject);
	PX_TextureFree(&pdesc->panel);
}


PX_Object* PX_Object_ColorPanelCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height)
{
	px_int radius;
	PX_Object_ColorPanel desc = {0}, * pdesc;
	PX_Object* pObject;
	if (width<64)
	{
		width = 64;
	}
	if (height<64)
	{
		height = 64;
	}
	radius = width > height ? height : width;
	pObject = PX_ObjectCreateEx(mp, Parent, x*1.f, y*1.f, 0, width*1.f, height*1.f, 0, PX_OBJECT_TYPE_COLORPANEL, 0, PX_Object_ColorPanelRender, PX_Object_ColorPanelFree, &desc, sizeof(desc));
	pdesc = PX_ObjectGetDesc(PX_Object_ColorPanel, pObject);
	pdesc->mp = mp;
	pdesc->color_hsv.a = 1;
	pdesc->color_rgb = PX_ColorHSVToRGB(pdesc->color_hsv);
	if (!PX_TextureCreate(mp, &pdesc->panel, radius, radius))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	PX_Object_ColorPanelUpdatePanelTexture(pObject);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDOWN, PX_Object_ColorPanelOnCursorDown, pObject);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORDRAG, PX_Object_ColorPanelOnCursorDrag, pObject);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORMOVE, PX_Object_ColorPanelOnCursorCancel, pObject);
	PX_ObjectRegisterEvent(pObject, PX_OBJECT_EVENT_CURSORUP, PX_Object_ColorPanelOnCursorCancel, pObject);
	return pObject;
}


PX_Object* PX_Designer_ColorPanelCreate(px_memorypool* mp, PX_Object* pparent, px_float x, px_float y, px_float width, px_float height, px_void* ptr)
{
	return PX_Object_ColorPanelCreate(mp, pparent, (px_int)x, (px_int)y, 200, 200);
}


PX_Designer_ObjectDesc PX_Object_ColorPanelDesignerInstall()
{
	PX_Designer_ObjectDesc desc;
	px_int i = 0;
	PX_memset(&desc, 0, sizeof(desc));
	PX_strcat(desc.Name, "colorpanel");

	desc.createfunc = PX_Designer_ColorPanelCreate;
	desc.type = PX_DESIGNER_OBJECT_TYPE_UI;

	PX_strcat(desc.properties[i].Name, "id");
	desc.properties[i].getstring = PX_Designer_GetID;
	desc.properties[i].setstring = PX_Designer_SetID;
	i++;

	PX_strcat(desc.properties[i].Name, "x");
	desc.properties[i].getfloat = PX_Designer_GetX;
	desc.properties[i].setfloat = PX_Designer_SetX;
	i++;

	PX_strcat(desc.properties[i].Name, "y");
	desc.properties[i].getfloat = PX_Designer_GetY;
	desc.properties[i].setfloat = PX_Designer_SetY;
	i++;

	PX_strcat(desc.properties[i].Name, "width");
	desc.properties[i].getfloat = PX_Designer_GetWidth;
	desc.properties[i].setfloat = PX_Designer_SetWidth;
	i++;

	PX_strcat(desc.properties[i].Name, "height");
	desc.properties[i].getfloat = PX_Designer_GetHeight;
	desc.properties[i].setfloat = PX_Designer_SetHeight;
	i++;

	return desc;
}

