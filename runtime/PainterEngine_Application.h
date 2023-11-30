#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H

#define PX_APPLICATION_NAME "PainterEngine"
#define PX_APPLICATION_MEMORYPOOL_STATIC_SIZE (1024*1024*64)
#define PX_APPLICATION_MEMORYPOOL_DYNAMIC_SIZE (1024*1024*48)
#define PX_APPLICATION_MEMORYPOOL_SPACE_SIZE (1024*1024*16)

#define PX_APPLICATION_MEMORYPOOL_ALL_SIZE (PX_APPLICATION_MEMORYPOOL_STATIC_SIZE+PX_APPLICATION_MEMORYPOOL_DYNAMIC_SIZE+PX_APPLICATION_MEMORYPOOL_SPACE_SIZE)
#include "PainterEngine_Runtime.h"
#include "platform/modules/px_file.h"
#include "platform/modules/px_audio.h"
#include "platform/modules/px_request.h"
#include "platform/modules/px_tcp.h"
#include "platform/modules/px_udp.h"

	typedef struct
	{
		px_color backgroundColor;
		PX_Object* object_root,*object_designer,*object_messagebox;
		PX_World world;
		PX_SoundPlay soundplay;
		PX_Runtime runtime;
		px_byte cache[PX_APPLICATION_MEMORYPOOL_ALL_SIZE];
	}PX_Application;

	extern PX_Application App;
	px_bool PainterEngine_Initialize(px_int screen_width, px_int screen_height);
	px_bool PainterEngine_InitializeWorld(px_int width, px_int height);
	px_bool PainterEngine_InitializeAudio();

	PX_Object* PainterEngine_GetRoot();
	px_surface* PainterEngine_GetSurface();
	PX_Runtime* PainterEngine_GetRuntime();
	px_memorypool* PainterEngine_GetDynamicMP();
	px_memorypool* PainterEngine_GetStaticMP();
	px_void PainterEngine_SetBackgroundColor(px_color color);
	PX_SoundPlay* PainterEngine_GetSoundPlay();

	px_void PainterEngine_DesignerInstall(PX_Designer_ObjectDesc desc);
	px_void PainterEngine_DesignerInstallDefault();
	px_void PainterEngine_LoadObjects();
	px_void PainterEngine_EnterDesignerMode();

	px_bool PX_ApplicationInitialize(PX_Application* App, px_int screen_Width, px_int screen_Height);
	px_void PX_ApplicationUpdate(PX_Application* App, px_dword elapsed);
	px_void PX_ApplicationRender(PX_Application* App, px_dword elapsed);
	px_void PX_ApplicationPostEvent(PX_Application* App, PX_Object_Event e);


#endif
