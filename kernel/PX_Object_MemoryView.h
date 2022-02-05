#ifndef PX_OBJECT_MEMORYVIEW_H
#define PX_OBJECT_MEMORYVIEW_H
#include "PX_Object.h"

typedef struct 
{
	px_void *pdata;
	px_int size;
	px_int cursor;
	PX_Object *scrollbar;
	px_dword address_offset;
	px_int mark_start_cursor;
	px_int mark_end_cursor;

	px_dword cursor_elapsed;

	px_color bordercolor;
	px_color backgroundcolor;
	px_color fontcolor;
	px_color cursorcolor;
	px_color selectcolor;
}PX_Object_MemoryView;

PX_Object *	PX_Object_MemoryViewCreate(px_memorypool *mp,PX_Object *pParent,px_int x,px_int y,px_int Width,px_int Height);
PX_Object_MemoryView *PX_Object_GetMemoryView(PX_Object *pObject);
px_void		PX_Object_MemoryViewSetFontColor(PX_Object *pObject,px_color Color);
px_void		PX_Object_MemoryViewSetBackgroundColor(PX_Object *pObject,px_color Color);
px_void		PX_Object_MemoryViewSetData(PX_Object *pObject,px_void *pdata,px_dword size);
px_bool		PX_Object_MemoryViewReadData(PX_Object* pObject,px_dword offset, px_void* poutdata, px_dword size);
#endif

