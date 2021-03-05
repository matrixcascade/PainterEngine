#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H

#include "PainterEngine_Startup.h"

typedef struct
{
	PX_Runtime runtime;
}PX_Application;

extern PX_Application App;

px_bool PX_ApplicationInitialize(PX_Application *App,px_int screen_Width,px_int screen_Height);
px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased);
px_void PX_ApplicationRender(PX_Application *App,px_dword elpased);
px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e);

#endif
