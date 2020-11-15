#ifndef PX_LIVEFRAMEWORK_H
#define PX_LIVEFRAMEWORK_H

#include "PX_LiveLayer.h"


typedef struct
{
	px_memorypool *mp;
	px_vector animation;
	px_vector layers;
}PX_LiveFramework;


px_bool PX_LiveFrameworkInitialize(px_memorypool *mp,PX_LiveFramework *plive);
px_bool PX_LiveFrameworkLoad(PX_LiveFramework *plive,const px_char json_content[]);
px_void PX_LiveFrameworkUpdate(PX_LiveFramework *plive,px_dword elpased);
px_void PX_LiveFrameworkRender(px_surface *psurface,PX_LiveFramework *plive,px_dword elpased);
px_bool PX_LiveFrameworkPlayAnimation(PX_LiveFramework *plive,const px_char name[]);

#endif