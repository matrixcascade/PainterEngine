#ifndef SOD_STARTUP_H
#define SOD_STARTUP_H

#include "SOD_Runtime.h"

typedef struct
{
	SOD_Runtime *pRuntime;
	px_texture background;
	PX_Object *root;
	PX_Object *btn_startgame;
	PX_Object *btn_Aboutgame;
	PX_Object *btn_exitgame;
}SOD_Startup;

px_bool SOD_StartupInitialize(SOD_Runtime *pruntime,SOD_Startup *Startup);
px_void SOD_StartupUpdate(SOD_Startup *Startup,px_dword elpased);
px_void SOD_StartupRender(SOD_Startup *Startup,px_dword elpased);
px_void SOD_StartupPostEvent(SOD_Startup *Startup,PX_Object_Event e);
#endif
