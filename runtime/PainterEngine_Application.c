#include "PainterEngine_Application.h"

extern int px_main();
//////////////////////////////////////////////////////////////////////////
PX_Application App;
PX_Object* root;

px_memorypool* mp,*mp_static;
px_int screen_width;
px_int screen_height;
px_int surface_width;
px_int surface_height;
px_surface* render_surface;
PX_ResourceLibrary *resource_library;
PX_SoundPlay* soundplay;
px_char PX_APPLICATION_NAME[64] = "PainterEngine";
//////////////////////////////////////////////////////////////////////////

px_void PainterEngine_Print(const px_char content[])
{
	if (App.object_printer->Visible==PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_Object_PrinterPrintText(App.object_printer, content);
}

px_void PainterEngine_PrintWithColor(const px_char content[],px_color clr)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_Object_PrinterPrintTextWithColor(App.object_printer, content,clr);
}

px_void PainterEngine_PrintClear()
{
	PX_Object_PrinterClear(App.object_printer);
}

px_void PainterEngine_PrintSetBackgroundColor(px_color clr)
{
	PX_Object_PrinterSetBackgroundColor(App.object_printer, clr);
}

px_void PainterEngine_PrintSetFontColor(px_color clr)
{
	PX_Object_PrinterSetFontColor(App.object_printer, clr);
}

px_void PainterEngine_PrintImage(const px_char path[])
{
	#ifdef PAINTERENGIN_FILE_SUPPORT
	PX_Object* pObject;
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	pObject=PX_Object_PrinterPrintImage(App.object_printer, PX_NULL);
	if (pObject)
	{
		PX_IO_Data data = PX_LoadFileToIOData(path);
		if (PX_strstr2(path, ".gif"))
		{
			PX_Object_ImageLoadGif(pObject, data.buffer, data.size);
		}
		else
		{
			PX_Object_ImageLoadTexture(pObject, data.buffer, data.size);
		}
		PX_Object_ImageAutoSize(pObject);
		PX_Object_PrinterUpdateAll(App.object_printer);
		PX_FreeIOData(&data);
	}
	#endif
}

px_void PainterEngine_DrawPixel( px_int x, px_int y, px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_SurfaceDrawPixel(PX_Object_PanelGetSurface(App.object_printer),  x, y, color);
}

px_void PainterEngine_DrawText(px_int x, px_int y, const px_char text[],PX_ALIGN align, px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_FontModuleDrawText(PX_Object_PanelGetSurface(App.object_printer), App.pfontmodule, x, y, align, text, color);
	
}

px_surface *PainterEngine_GetDrawSurface()
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	return PX_Object_PanelGetSurface(App.object_printer);
}
px_int PainterEngine_GetSurfaceWidth()
{
		return App.runtime.surface_width;
}
px_int PainterEngine_GetSurfaceHeight()
{
	return App.runtime.surface_height;
}

px_int PainterEngine_GetScreenWidth()
{
	return screen_width;
}
px_int PainterEngine_GetScreenHeight()
{
	return screen_height;
}

px_void PainterEngine_DrawTexture(px_texture *ptexture,px_int x,px_int y,PX_ALIGN align)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_TextureRender(PX_Object_PanelGetSurface(App.object_printer), ptexture, x, y, align, 0);
}

px_void PainterEngine_DrawLine(px_int x1,px_int y1,px_int x2,px_int y2,px_int linewidth,px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_GeoDrawLine(PX_Object_PanelGetSurface(App.object_printer), x1, y1, x2, y2, linewidth, color);
}


px_void PainterEngine_DrawRect(px_int x,px_int y,px_int width,px_int height,px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_GeoDrawRect(PX_Object_PanelGetSurface(App.object_printer), x, y, x+width-1, y+height-1, color);
}

px_void PainterEngine_DrawCircle(px_int x,px_int y,px_int radius,px_int linewidth,px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_GeoDrawCircle(PX_Object_PanelGetSurface(App.object_printer), x, y, radius, linewidth, color);
}

px_void PainterEngine_DrawSolidCircle(px_int x, px_int y, px_int radius,  px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_GeoDrawSolidCircle(PX_Object_PanelGetSurface(App.object_printer), x, y, radius, color);
}

px_void PainterEngine_DrawSector(px_int x, px_int y, px_int radius_inside, px_int radius_outside, px_int start_angle, px_int end_angle, px_color color)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_GeoDrawSector(PX_Object_PanelGetSurface(App.object_printer), x, y, (px_float)radius_inside, (px_float)radius_outside, color, start_angle, end_angle);
}



px_void PainterEngine_SetFontSize(px_int size)
{
if(App.pfontmodule)
		PX_FontModuleSetSize(App.pfontmodule, size);
}

PX_Object* PainterEngine_PrintButton(px_int width,px_int height,const px_char text[])
{
	PX_Object* pObject;
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}

	pObject = PX_Object_PrinterPrintButton(App.object_printer, width, height, text);
	return pObject;
}

PX_Object* PainterEngine_PrintProcessBar()
{
	PX_Object* pObject;
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}

	pObject = PX_Object_PrinterPrintProcessBar(App.object_printer);
	return pObject;
}

PX_Object* PainterEngine_PrintSpace(px_int height)
{
	PX_Object* pObject;
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}

	pObject = PX_Object_PrinterPrintSpace(App.object_printer,height);
	return pObject;
}

px_void  PainterEngine_PrintObject(PX_Object* pNewObject)
{
	if (App.object_printer->Visible == PX_FALSE)
	{
		PX_ObjectSetVisible(App.object_printer, PX_TRUE);
	}
	PX_Object_PrinterPrintObject(App.object_printer, pNewObject);
}

PX_Object*  PainterEngine_PrintGetArea()
{
	return PX_Object_PrinterGetArea(App.object_printer);
}

const px_char* PainterEngine_Language(const px_char tr[])
{
	if (App.language.mp)
	{
		const px_char *ptr=PX_JsonGetString(&App.language, tr);
		if (!ptr[0])
			ptr = tr;
		return ptr;
	}
	else
	{
		return tr;
	}
}

px_void PainterEngine_PrintSetCodepage(PX_FONTMODULE_CODEPAGE codepage)
{
	if(App.pfontmodule)
		PX_FontModuleSetCodepage(App.pfontmodule, codepage);
}

PX_FontModule* PainterEngine_GetFontModule()
{
	return App.pfontmodule;
}

px_bool PainterEngine_LoadFontModule(const px_char path[],PX_FONTMODULE_CODEPAGE codepage,px_int size)
{
#ifdef PAINTERENGIN_FILE_SUPPORT
	if (!App.pfontmodule)
	{
		if (PX_LoadFontModuleFromTTF(&App.runtime.mp_static, &App.fontmodule, path, codepage, size))
		{
			App.pfontmodule = &App.fontmodule;
			return PX_TRUE;
		}
		else
		{
			return PX_FALSE;
		}
	}
	return PX_FALSE;
#else
	return PX_FALSE;
	#endif
}

px_void PainterEngine_SetWindowText(const px_char text[])
{
	PX_strcpy(PX_APPLICATION_NAME, text, sizeof(PX_APPLICATION_NAME));
}

px_bool PainterEngine_Initialize(px_int _screen_width,px_int _screen_height)
{
	PX_Runtime* runtime = &App.runtime;
	if (!PX_RuntimeInitialize(runtime, _screen_width, _screen_height, _screen_width, _screen_height, App.cache, sizeof(App.cache), PX_APPLICATION_MEMORYPOOL_STATIC_SIZE, PX_APPLICATION_MEMORYPOOL_DYNAMIC_SIZE))
		return PX_FALSE;
#ifdef PX_AUDIO_H
	PX_SoundPlayInitialize(mp, &App.soundplay);
	PainterEngine_InitializeAudio();
#endif
	if (_screen_width == 0 || _screen_height == 0)
		return PX_TRUE;
#ifdef PAINTERENGIN_FILE_SUPPORT
	
	App.pfontmodule = PX_NULL;
	if(!PX_JsonInitialize(&runtime->mp_static, &App.language))
	{
		return PX_FALSE;
	}
	if (!PX_LoadJsonFromFile(&App.language, "assets/language.json"))
	{
		PX_JsonFree(&App.language);
		PX_memset(&App.language, 0, sizeof(App.language));
	}
#endif

	App.object_root = PX_ObjectCreateRoot(&runtime->mp_static);
	App.object_printer=PX_Object_PrinterCreate(&runtime->mp, App.object_root, 0, 0, _screen_width, _screen_height, App.pfontmodule);
	PX_Object_PanelAttachObject(App.object_printer, PX_ObjectGetFreeDescIndex(App.object_printer));
	PX_Object_PrinterSetBackgroundColor(App.object_printer, PX_COLOR_NONE);
	App.object_printer->Visible = PX_FALSE;
	

	App.object_messagebox = PX_Object_MessageBoxCreate(&runtime->mp_static, App.object_root, 0);
	root = App.object_root;
	mp = &runtime->mp_dynamic;
	mp_static=&runtime->mp_static;
	screen_width = _screen_width;
	screen_height = _screen_height;
	surface_width = App.runtime.surface_width;
	surface_height = App.runtime.surface_height;
	render_surface=&App.runtime.RenderSurface;
	resource_library =&App.runtime.ResourceLibrary;
	soundplay = &App.soundplay;

	return PX_TRUE;
}


PX_Object* PainterEngine_GetRoot()
{
	return App.object_root;
}

px_surface * PainterEngine_GetSurface()
{
	return &App.runtime.RenderSurface;
}

PX_Runtime* PainterEngine_GetRuntime()
{
	return &App.runtime;
}

PX_ResourceLibrary* PainterEngine_GetResourceLibrary()
{
	return &App.runtime.ResourceLibrary;
}

px_memorypool * PainterEngine_GetDynamicMP()
{
	return &App.runtime.mp_dynamic;
}
px_memorypool* PainterEngine_GetStaticMP()
{
	return &App.runtime.mp_static;
}


px_bool PainterEngine_InitializeAudio()
{
#ifdef PX_AUDIO_H
	static px_bool audioInit = PX_FALSE;
	if (!audioInit)
	{
		audioInit = PX_TRUE;
		return PX_AudioInitialize(&App.soundplay);
	}
	return PX_TRUE;
#else
	return PX_FALSE;
#endif //  PX_AUDIO_H
}

px_void PainterEngine_SetBackgroundColor(px_color color)
{
	App.backgroundColor = color;
}

px_void PainterEngine_SetBackgroundTexture(px_texture *pTexture)
{
	App.pBackgroudTexture = pTexture;
}

PX_SoundPlay* PainterEngine_GetSoundPlay()
{
	return &App.soundplay;
}


//////////////////////////////////////////////////////////////////////////
//Functions


px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int _screen_width,px_int _screen_height)
{
	PX_memset(pApp, 0, sizeof(PX_Application));
	screen_width = _screen_width;
	screen_height = _screen_height;
	PainterEngine_SetBackgroundColor(PX_COLOR_BACKGROUNDCOLOR);
	PX_srand(314159);
	px_main();
	
	if (pApp->runtime.window_height&&pApp->runtime.window_width&& pApp->runtime.surface_height&& pApp->runtime.surface_width)
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elapsed)
{
	PX_ObjectUpdate(pApp->object_root, elapsed);
}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elapsed)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime,pApp->backgroundColor);
	if(pApp->pBackgroudTexture)
	{
		PX_TextureRender(pRenderSurface, pApp->pBackgroudTexture, 0, 0, PX_ALIGN_LEFTTOP, 0);
	}
	PX_ObjectRender(pRenderSurface, pApp->object_root,elapsed);
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ObjectPostEvent(pApp->object_root,e);
}

