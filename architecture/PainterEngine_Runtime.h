#ifndef PAINTERENGINE_RUNTIME_H
#define PAINTERENGINE_RUNTIME_H

#include "../kernel/PX_Kernel.h"

#define PE_MEMORY_CALC_SIZE 1024*1024*2//2M

typedef struct  
{
	px_int window_width;
	px_int window_height;
	px_int surface_width;
	px_int surface_height;
	px_memorypool		mp;
	px_memorypool		mp_ui;
	px_memorypool		mp_resources;
	px_memorypool		mp_game;
	PX_ResourceLibrary	ResourceLibrary;
	px_surface			RenderSurface;

	//////////////////////////////////////////////////////////////////////////
	PX_FontModule fontmodule;
	PX_SoundPlay  soundplay;


}PX_Runtime;


px_bool PX_RuntimeInitialize(PX_Runtime *pe,px_int surface_width,px_int surface_height,px_int window_width,px_int window_height,px_void *runtime_memoryPtr,px_uint size,px_uint ui_size,px_uint resource_size,px_uint game_size);
px_bool PX_RuntimeResize(PX_Runtime *pe,px_int surface_width,px_int surface_height,px_int window_width,px_int window_height);
px_void PX_RuntimeReset(PX_Runtime *pe);
px_memorypool PX_RuntimeCreateCalcMemoryPool(PX_Runtime *pe);
px_void PX_RuntimeFreeCalcMemoryPool(PX_Runtime *pe,px_memorypool mp);
px_surface *PX_RuntimeGetRenderSurface(PX_Runtime *pe);
px_void PX_RuntimeRenderClear(PX_Runtime *runtime,px_color color);
#endif
