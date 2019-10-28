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



px_bool PX_RuntimeInitialize(PX_Runtime *pe,px_int width,px_int height,px_void *runtime_memoryPtr,px_uint size,px_uint ui_size,px_uint resource_size,px_uint game_size)
{
	pe->width=width;
	pe->height=height;

	pe->mp=MP_Create(runtime_memoryPtr,size);
	pe->mp_ui=MP_Create(MP_Malloc(&pe->mp,ui_size),ui_size);
	pe->mp_resources=MP_Create(MP_Malloc(&pe->mp,resource_size),resource_size);
	pe->mp_game=MP_Create(MP_Malloc(&pe->mp,game_size),game_size);


	MP_ErrorCatch(&pe->mp_ui,PE_MP_ErrorCall);
	MP_ErrorCatch(&pe->mp_resources,PE_MP_ErrorCall);
	MP_ErrorCatch(&pe->mp_game,PE_MP_ErrorCall);

	//resources
	if(!PX_ResourceLibraryInit(&pe->mp_resources,&pe->ResourceLibrary))return PX_FALSE;
	//surface
	if(!PX_SurfaceCreate(&pe->mp,width,height,&pe->RenderSurface))
	{
		return PX_FALSE;
	}
	PX_SurfaceClear(&pe->RenderSurface,0,0,pe->RenderSurface.width-1,pe->RenderSurface.height-1,PX_COLOR(255,255,255,255));
	return PX_TRUE;
}

px_memorypool PX_RuntimeCreateCalcMemoryPool(PX_Runtime *pe)
{
	return MP_Create(MP_Malloc(&pe->mp,PE_MEMORY_CALC_SIZE),PE_MEMORY_CALC_SIZE);
}

px_void PX_RuntimeFreeCalcMemoryPool(PX_Runtime *pe,px_memorypool mp)
{
	MP_Free(&pe->mp,mp.StartAddr);
}
