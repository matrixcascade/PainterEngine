#include "PainterEngine_Runtime.h"


px_void PE_MP_ErrorCall(PX_MEMORYPOOL_ERROR err)
{
	switch(err)
	{
	case PX_MEMORYPOOL_ERROR_INVALID_ACCESS:
		PX_ERROR("Memory access error");
		break;
	case PX_MEMORYPOOL_ERROR_INVALID_ADDRESS:
		PX_ERROR("Memory address error");
		break;
	case PX_MEMORYPOOL_ERROR_OUTOFMEMORY:
		PX_ERROR("Out of memory");
		break;
	}
}



px_bool PX_RuntimeInitialize(PX_Runtime *pe,px_int surface_width,px_int surface_height,px_int window_width,px_int window_height,px_void *runtime_memoryPtr,px_uint size,px_uint ui_size,px_uint resource_size,px_uint game_size)
{
	PX_memset(pe,0,sizeof(PX_Runtime));
	pe->surface_width=surface_width;
	pe->surface_height=surface_height;
	pe->window_width=window_width;
	pe->window_height=window_height;

	pe->mp=MP_Create(runtime_memoryPtr,size);
	pe->mp_ui=MP_Create(MP_Malloc(&pe->mp,ui_size),ui_size);
	pe->mp_resources=MP_Create(MP_Malloc(&pe->mp,resource_size),resource_size);
	pe->mp_game=MP_Create(MP_Malloc(&pe->mp,game_size),game_size);


	MP_ErrorCatch(&pe->mp_ui,PE_MP_ErrorCall);
	MP_ErrorCatch(&pe->mp_resources,PE_MP_ErrorCall);
	MP_ErrorCatch(&pe->mp_game,PE_MP_ErrorCall);

	//resources
	if(!PX_ResourceLibraryInitialize(&pe->mp_resources,&pe->ResourceLibrary))return PX_FALSE;
	//surface
	if (pe->surface_width==0||pe->surface_height==0)
	{
		pe->RenderSurface.height=0;
		pe->RenderSurface.width=0;
		pe->RenderSurface.surfaceBuffer=PX_NULL;
		pe->RenderSurface.MP=PX_NULL;
	}
	else
	{
		if(!PX_SurfaceCreate(&pe->mp,surface_width,surface_height,&pe->RenderSurface))
		{
			return PX_FALSE;
		}
		PX_SurfaceClear(&pe->RenderSurface,0,0,pe->RenderSurface.width-1,pe->RenderSurface.height-1,PX_COLOR(255,255,255,255));
	}

	
	
	return PX_TRUE;
}


px_bool PX_RuntimeResize(PX_Runtime *pe,px_int surface_width,px_int surface_height,px_int window_width,px_int window_height)
{
	if (PX_SurfaceMemorySize(surface_width,surface_height)+64>(px_int)pe->mp.FreeSize)
	{
		PX_ASSERT();//out of memory
		return PX_FALSE;
	}

	PX_SurfaceFree(&pe->RenderSurface);

	pe->surface_width=surface_width;
	pe->surface_height=surface_height;
	pe->window_width=window_width;
	pe->window_height=window_height;

	if(!PX_SurfaceCreate(&pe->mp,surface_width,surface_height,&pe->RenderSurface))
	{
		PX_ASSERT();
		return PX_FALSE;
	}
	PX_SurfaceClear(&pe->RenderSurface,0,0,pe->RenderSurface.width-1,pe->RenderSurface.height-1,PX_COLOR(255,255,255,255));

	return PX_TRUE;
}


px_void PX_RuntimeReset(PX_Runtime *pe)
{
	MP_Reset(&pe->mp_game);
	MP_Reset(&pe->mp_resources);
	MP_Reset(&pe->mp_ui);
	PX_ResourceLibraryInitialize(&pe->mp_resources,&pe->ResourceLibrary);
	PX_SurfaceClear(&pe->RenderSurface,0,0,pe->RenderSurface.width-1,pe->RenderSurface.height-1,PX_COLOR(0,0,0,0));
}

px_memorypool PX_RuntimeCreateCalcMemoryPool(PX_Runtime *pe)
{
	return MP_Create(MP_Malloc(&pe->mp,PE_MEMORY_CALC_SIZE),PE_MEMORY_CALC_SIZE);
}

px_void PX_RuntimeFreeCalcMemoryPool(PX_Runtime *pe,px_memorypool mp)
{
	MP_Free(&pe->mp,mp.StartAddr);
}


px_surface * PX_RuntimeGetRenderSurface(PX_Runtime *pe)
{
	return &pe->RenderSurface;
}

px_void PX_RuntimeRenderClear(PX_Runtime *runtime,px_color color)
{
	PX_SurfaceClear(&runtime->RenderSurface,0,0,runtime->RenderSurface.width-1,runtime->RenderSurface.height-1,color);
}
