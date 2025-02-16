#ifndef PX_OBJECT_COLLAPSED
#define PX_OBJECT_COLLAPSED
#include "PX_Object.h"
typedef struct
{
	PX_Object* title, * collapse, * scrollarea;
}PX_Object_Collapse;

PX_Object* PX_Object_CollapseCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, const px_char* title, PX_FontModule* fontModule);


#endif // !PX_OBJECT_COLLAPSED
