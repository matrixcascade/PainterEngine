#include "PX_LiveFramework.h"
px_bool PX_LiveFrameworkInitialize(px_memorypool *mp,PX_LiveFramework *plive)
{
	plive->mp=mp;
	if(!PX_VectorInitialize(mp,&plive->animation,sizeof(PX_LiveAnimation),0))return PX_FALSE;
	if(!PX_VectorInitialize(mp,&plive->layers,sizeof(PX_LiveLayer),1))return PX_FALSE;
	return PX_TRUE;
}

px_bool PX_LiveFrameworkLoad(PX_LiveFramework *plive,const px_char json_content[])
{
	return PX_TRUE;
}

px_void PX_LiveFrameworkUpdate(PX_LiveFramework *plive,px_dword elpased)
{
	
}

px_void PX_LiveFrameworkRender(px_surface *psurface,PX_LiveFramework *plive,px_dword elpased)
{

}

px_bool PX_LiveFrameworkPlayAnimation(PX_LiveFramework *plive,const px_char name[])
{
	return PX_TRUE;
}

