#ifndef PX_OBJECT_PARTICAL_H
#define PX_OBJECT_PARTICAL_H
#include "PX_Object.h"
#include "PX_Partical.h"

typedef struct 
{
	PX_ParticalLauncher launcher;
}PX_Object_Partical;

PX_Object* PX_Object_ParticalCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, PX_ParticalLauncher_InitializeInfo ParticalInfo);

#endif



