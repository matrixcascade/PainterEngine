#ifndef PAINTERENGINE_RUNTIME_H
#define PAINTERENGINE_RUNTIME_H

#include "../kernel/PX_Kernel.h"


typedef struct  
{
	px_int window_width;
	px_int window_height;
	px_int surface_width;
	px_int surface_height;
	px_memorypool		mp;
	px_memorypool		mp_static;
	px_memorypool		mp_dynamic;
	PX_ResourceLibrary	ResourceLibrary;
	px_surface			RenderSurface;
}PX_Runtime;


px_bool PX_RuntimeInitialize(PX_Runtime *pe,px_int surface_width,px_int surface_height,px_int window_width,px_int window_height,px_void *runtime_memoryPtr,px_uint size,px_uint static_size,px_uint dynamic_size);
px_bool PX_RuntimeResize(PX_Runtime *pe,px_int surface_width,px_int surface_height,px_int window_width,px_int window_height);
px_void PX_RuntimeReset(PX_Runtime *pe);
px_surface *PX_RuntimeGetRenderSurface(PX_Runtime *pe);
px_void PX_RuntimeRenderClear(PX_Runtime *runtime,px_color color);

#endif
