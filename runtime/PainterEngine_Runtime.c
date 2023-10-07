#include "PainterEngine_Runtime.h"


px_void PX_Runtime_MP_ErrorCall(px_void *ptr,PX_MEMORYPOOL_ERROR err)
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



px_bool PX_RuntimeInitialize(PX_Runtime *pe,px_int surface_width,px_int surface_height,px_int window_width,px_int window_height,px_void *runtime_memoryPtr,px_uint size, px_uint static_size, px_uint dynamic_size)
{
	PX_memset(pe,0,sizeof(PX_Runtime));
	pe->surface_width=surface_width;
	pe->surface_height=surface_height;
	pe->window_width=window_width;
	pe->window_height=window_height;

	pe->mp=MP_Create(runtime_memoryPtr,size);

	pe->mp_static=MP_Create(MP_Malloc(&pe->mp, static_size), static_size);
	pe->mp_dynamic=MP_Create(MP_Malloc(&pe->mp, dynamic_size), dynamic_size);

	MP_ErrorCatch(&pe->mp_static,PX_Runtime_MP_ErrorCall,pe);
	MP_ErrorCatch(&pe->mp_dynamic,PX_Runtime_MP_ErrorCall,pe);

	//resources
	if(!PX_ResourceLibraryInitialize(&pe->mp_static,&pe->ResourceLibrary))return PX_FALSE;
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
	MP_Reset(&pe->mp_dynamic);
	MP_Reset(&pe->mp_static);

	PX_ResourceLibraryInitialize(&pe->mp_static,&pe->ResourceLibrary);
	PX_SurfaceClear(&pe->RenderSurface,0,0,pe->RenderSurface.width-1,pe->RenderSurface.height-1,PX_COLOR(0,0,0,0));
}


px_surface * PX_RuntimeGetRenderSurface(PX_Runtime *pe)
{
	return &pe->RenderSurface;
}

px_void PX_RuntimeRenderClear(PX_Runtime *runtime,px_color color)
{
	PX_SurfaceClear(&runtime->RenderSurface,0,0,runtime->RenderSurface.width-1,runtime->RenderSurface.height-1,color);
}
