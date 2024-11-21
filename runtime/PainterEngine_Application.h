#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H

#define PX_APPLICATION_NAME "PainterEngine"
#define PX_APPLICATION_MEMORYPOOL_STATIC_SIZE (1024*1024*64)
#define PX_APPLICATION_MEMORYPOOL_DYNAMIC_SIZE (1024*1024*32)
#define PX_APPLICATION_MEMORYPOOL_SPACE_SIZE (1024*1024*16)

#define PX_APPLICATION_MEMORYPOOL_ALL_SIZE (PX_APPLICATION_MEMORYPOOL_STATIC_SIZE+PX_APPLICATION_MEMORYPOOL_DYNAMIC_SIZE+PX_APPLICATION_MEMORYPOOL_SPACE_SIZE)
#include "PainterEngine_Runtime.h"
#include "platform/modules/px_file.h"
#include "platform/modules/px_time.h"
#include "platform/modules/px_audio.h"
#include "platform/modules/px_request.h"
#include "platform/modules/px_tcp.h"
#include "platform/modules/px_udp.h"

	typedef struct
	{
		px_color backgroundColor;
		px_texture* pBackgroudTexture;
		PX_Object* object_root,*object_messagebox,*object_printer,*object_panel;
		PX_SoundPlay soundplay;
		PX_Runtime runtime;
		PX_Json language;
		PX_FontModule fontmodule,*pfontmodule;
		px_byte cache[PX_APPLICATION_MEMORYPOOL_ALL_SIZE];
	}PX_Application;

	extern PX_Application App;
	px_bool PainterEngine_Initialize(px_int screen_width, px_int screen_height);
	px_bool PainterEngine_InitializeAudio();

	PX_Object* PainterEngine_GetRoot();
	px_surface* PainterEngine_GetSurface();
	PX_Runtime* PainterEngine_GetRuntime();
	px_memorypool* PainterEngine_GetDynamicMP();
	px_memorypool* PainterEngine_GetStaticMP();
	px_void PainterEngine_SetBackgroundColor(px_color color);
	px_void PainterEngine_SetBackgroundTexture(px_texture* pTexture);
	PX_SoundPlay* PainterEngine_GetSoundPlay();
	PX_ResourceLibrary* PainterEngine_GetResourceLibrary();

	px_void PainterEngine_Print(const px_char content[]);
	px_void PainterEngine_PrintClear();
	px_void PainterEngine_PrintImage(const px_char path[]);
	px_void PainterEngine_SetFontSize(px_int size);
	PX_Object* PainterEngine_PrintButton(px_int width, px_int height, const px_char text[]);
	PX_Object* PainterEngine_PrintProcessBar();
	PX_Object* PainterEngine_PrintSpace(px_int height);
	px_void  PainterEngine_PrintObject(PX_Object* pNewObject);
	px_void  PainterEngine_PrintSetCodepage(PX_FONTMODULE_CODEPAGE codepage);
	px_void PainterEngine_PrintSetBackgroundColor(px_color clr);
	px_void PainterEngine_PrintSetFontColor(px_color clr);
	PX_Object* PainterEngine_PrintGetArea();
	PX_FontModule* PainterEngine_GetFontModule();
	const px_char* PainterEngine_Language(const px_char tr[]);

	px_bool PainterEngine_LoadFontModule(const px_char path[], PX_FONTMODULE_CODEPAGE codepage, px_int size);
	px_void PainterEngine_DrawTexture(px_texture* ptexture, px_int x, px_int y, PX_ALIGN align);
	px_void PainterEngine_DrawLine(px_int x1, px_int y1, px_int x2, px_int y2, px_int linewidth, px_color color);
	px_void PainterEngine_DrawRect(px_int x, px_int y, px_int width, px_int height, px_color color);
	px_void PainterEngine_DrawCircle(px_int x, px_int y, px_int radius, px_int linewidth, px_color color);
	px_void PainterEngine_DrawSolidCircle(px_int x, px_int y, px_int radius, px_color color);
	px_void PainterEngine_DrawSector(px_int x, px_int y, px_int radius_inside, px_int radius_outside, px_int start_angle, px_int end_angle, px_color color);
	px_void PainterEngine_DrawPixel(px_int x, px_int y, px_color color);
	px_void PainterEngine_DrawText(px_int x, px_int y, const px_char text[], PX_ALIGN align, px_color color);
	

	px_bool PX_ApplicationInitialize(PX_Application* App, px_int screen_Width, px_int screen_Height);
	px_void PX_ApplicationUpdate(PX_Application* App, px_dword elapsed);
	px_void PX_ApplicationRender(PX_Application* App, px_dword elapsed);
	px_void PX_ApplicationPostEvent(PX_Application* App, PX_Object_Event e);

	


#endif
