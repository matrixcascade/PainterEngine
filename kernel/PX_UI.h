#ifndef PAINTERENGINE_RUNTIME_H
#define PAINTERENGINE_RUNTIME_H

#include "../kernel/PX_Json.h"
#include "../kernel/PX_Object.h"



typedef struct
{
	px_int allocID;
	px_float z;
	px_vector Objects;//PX_Object *
}PX_UI;

px_bool PX_UIInitialize(px_memorypool *mp,PX_UI *ui);
px_void PX_UIUpdate(PX_UI *ui,px_dword elpased);
px_void PX_UIRender(px_surface *psurface,PX_UI *ui,px_dword elpased);
px_void PX_UIFree(PX_UI *ui);

#endif