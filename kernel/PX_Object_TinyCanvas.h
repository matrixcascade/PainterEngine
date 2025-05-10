#ifndef PX_OBJECT_TINYPANEL_H
#define PX_OBJECT_TINYPANEL_H
#include "PX_Object.h"

#define PX_TINYCANVASVM_MAX_COUNT 1024
#define PX_TINYCANVASVM_MAX_TOOLS_COUNT 64
#define PX_TINYCANVASVM_MAX_TOOL_NAME 32
#define PX_TINYCANVASVM_MAX_DETAIL_SIZE PX_CANVASVM_MAX_COUNT
#define PX_TINYCANVASVM_STEP_DIV 8
#define PX_TINYCANVASVM_LAYER_MAX 8
#define PX_TINYCANVASVM_LAYER_NAME_LENGTH 64

#define PX_OBJECT_EVENT_TINYCANVAS_PAINTBEGIN 0x1000
#define PX_OBJECT_EVENT_TINYCANVAS_PAINTEND   0x1001
typedef enum
{
	PX_TinyCanvasVM_State_Standby,
	PX_TinyCanvasVM_State_Painting,
}PX_TinyCanvasVM_State;

typedef struct
{
	px_float32 x, y, z;
}PX_TinyCanvasNode;

typedef px_void(*pfun_PX_TinyCanvasVMOnPaint)(px_surface* psurface, px_float x, px_float y, px_float size, px_color color, px_void* ptr);

typedef struct
{
	px_char name[PX_TINYCANVASVM_MAX_TOOL_NAME];
	pfun_PX_TinyCanvasVMOnPaint paint;
	px_void* ptr;
}PX_TinyCanvasVMTool;


typedef struct
{
	px_memorypool* mp;
	PX_TinyCanvasVMTool tools[PX_CANVASVM_MAX_TOOLS_COUNT];
	PX_TinyCanvasNode currentPath[PX_CANVASVM_MAX_COUNT];
	px_int currentPathCount;
	px_float reg_x,reg_y;
	px_point reg_lastCursorPos;
	px_int   reg_state_id;
	px_int   reg_tools_count;
	px_int   reg_tool;
	px_float reg_size;
	px_color reg_color;
	px_float reg_filter_radius;
	PX_TinyCanvasVM_State reg_state;

	px_texture rendersurface;
}PX_Object_TinyCanvas;

PX_Object* PX_Object_TinyCanvasAttachObject(PX_Object* pObject, px_int attachIndex);
PX_Object* PX_Object_TinyCanvasCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height);
px_surface* PX_Object_TinyCanvasGetSurface(PX_Object* pObject);
px_void PX_Object_TinyCanvasSetColor(PX_Object* pObject, px_color color);
px_void PX_Object_TinyCanvasSetTool(PX_Object* pObject, const px_char name[PX_TINYCANVASVM_MAX_TOOL_NAME]);
px_void PX_Object_TinyCanvasSetSize(PX_Object* pObject, px_float size);
px_void PX_Object_TinyCanvasSetFilter(PX_Object* pObject, px_float filter);
px_void PX_Object_TinyCanvasClear(PX_Object* pObject);
#endif

