#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H
#include "PainterEngine_Startup.h"

//////////////////////////////////////////////////////////////////////////
//Configures
#define  PX_WINDOW_NAME     ""

#define  PX_WINDOW_WIDTH    800
#define  PX_WINDOW_HEIGHT   600

//memorypool for runtime(bytes)
#define PX_MEMORY_UI_SIZE (1024*1024*8)
#define PX_MEMORY_RESOURCES_SIZE (1024*1024*8)
#define PX_MEMORY_GAME_SIZE (1024*1024)
//////////////////////////////////////////////////////////////////////////

typedef struct
{
	PX_Instance Instance;
}PX_Application;

extern PX_Application App;

px_bool PX_ApplicationInitialize(PX_Application *App);
px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased);
px_void PX_ApplicationRender(PX_Application *App,px_dword elpased);
px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e);

#endif
