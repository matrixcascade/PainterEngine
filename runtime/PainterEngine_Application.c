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
//////////////////////////////////////////////////////////////////////////

PX_OBJECT_EVENT_FUNCTION(PainterEngine_DesignerOnSave)
{
	px_string json;
	PX_StringInitialize(mp, &json);
	PX_Object_DesignerExport(App.object_designer, &json);
	if (PX_SaveDataToFile(json.buffer,PX_strlen(json.buffer),"assets/objects.json"))
	{
		PX_Object_MessageBoxAlertOk(App.object_messagebox, "save to assets/objects.json", 0, 0);
	}
	else
	{
		PX_Object_MessageBoxAlertOk(App.object_messagebox, "failed to save assets/objects.json", 0, 0);
	}
	PX_StringFree(&json);
}

px_void PainterEngine_OnDesignerLoad(px_void* buffer, px_int size, px_void* ptr)
{
	PX_Object_DesignerImport(App.object_designer, (const px_char*)buffer);
}

px_char painterengine_loadbuffer[1024 * 1024 * 8];
PX_OBJECT_EVENT_FUNCTION(PainterEngine_DesignerOnOpen)
{
	PX_RequestData("open", painterengine_loadbuffer, sizeof(painterengine_loadbuffer), 0, PainterEngine_OnDesignerLoad);
}

px_bool PainterEngine_Initialize(px_int _screen_width,px_int _screen_height)
{
	PX_Runtime* runtime = &App.runtime;
	if (!PX_RuntimeInitialize(runtime, _screen_width, _screen_height, _screen_width, _screen_height, App.cache, sizeof(App.cache), PX_APPLICATION_MEMORYPOOL_STATIC_SIZE, PX_APPLICATION_MEMORYPOOL_DYNAMIC_SIZE))
		return PX_FALSE;

	App.object_root = PX_ObjectCreateRoot(&runtime->mp_dynamic);
	App.object_designer = PX_Object_DesignerCreate(&runtime->mp, App.object_root, App.object_root, PX_NULL, PX_NULL, PX_NULL);
	PX_ObjectRegisterEvent(App.object_designer, PX_OBJECT_EVENT_SAVE, PainterEngine_DesignerOnSave, PX_NULL);
	PX_ObjectRegisterEvent(App.object_designer, PX_OBJECT_EVENT_OPEN, PainterEngine_DesignerOnOpen, PX_NULL);
	App.object_messagebox = PX_Object_MessageBoxCreate(&runtime->mp, App.object_root, 0);
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

px_bool PainterEngine_InitializeWorld(px_int width, px_int height)
{
	if (!PX_WorldInitialize(&App.runtime.mp_dynamic, &App.world, width, height, App.runtime.surface_width, App.runtime.surface_height, 2 * 1024 * 1024))
		return PX_FALSE;
	PX_Object_DesignerBindWorld(App.object_designer, &App.world);
	return PX_TRUE;
}
px_int PainterEngine_GetViewWidth()
{
	return App.runtime.window_width;
}

px_int PainterEngine_GetViewHeight()
{
	return App.runtime.window_height;
}

px_int PainterEngine_GetSurfaceWidth()
{
	return App.runtime.window_width;
}

px_int PainterEngine_GetSurfaceHeight()
{
	return App.runtime.surface_width;
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
	return PX_AudioInitialize(&App.soundplay);
}

px_void PainterEngine_SetBackgroundColor(px_color color)
{
	App.backgroundColor = color;
}

PX_SoundPlay* PainterEngine_GetSoundPlay()
{
	return &App.soundplay;
}


//////////////////////////////////////////////////////////////////////////
//Functions

px_void PainterEngine_LoadObjects()
{
#ifdef PAINTERENGIN_FILE_SUPPORT
	PX_LoadUIFormFile(&App.runtime.mp_dynamic, App.object_designer, App.object_root,PX_NULL, "assets/objects.json");
#endif
}

px_void PainterEngine_EnterDesignerMode()
{
	PX_Object_DesignerEnable(App.object_designer);
}

px_void PainterEngine_DesignerInstall(PX_Designer_ObjectDesc desc)
{
	if (App.object_designer)
	{
		PX_Object_DesignerAddObjectDescription(App.object_designer, &desc);
	}
}

px_void PainterEngine_DesignerInstallDefault()
{
	if (App.object_designer)
	{
		PX_Object_Designer* pDesc = PX_ObjectGetDesc(PX_Object_Designer, App.object_designer);
		PX_Object_DesignerDefaultInstall(&pDesc->ObjectDesc);
	}
}

px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_memset(pApp, 0, sizeof(PX_Application));
	screen_width = screen_width;
	screen_height = screen_height;
	PainterEngine_SetBackgroundColor(PX_COLOR_BACKGROUNDCOLOR);

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
	PX_ObjectRender(pRenderSurface, pApp->object_root,elapsed);
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ObjectPostEvent(pApp->object_root,e);


}

