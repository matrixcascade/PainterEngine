#ifndef PX_CANVASVM_H
#define PX_CANVASVM_H

#define PX_CANVASVM_MAX_COUNT 8192
#define PX_CANVASVM_MAX_TOOLS_COUNT 64
#define PX_CANVASVM_MAX_TOOL_NAME 32
typedef struct
{
	px_float x, y, z;
	px_float v;
	px_dword elapsed;
}PX_CanvasNode;

typedef px_void (*pfun_PX_CanvasVMOnBegin)(px_surface* ppreview_surface, px_float x, px_float y, px_float z, px_void* ptr);
typedef px_void(*pfun_PX_CanvasVMOnMove)(px_surface* ppreview_psurface, px_float x, px_float y, px_float z, px_void* ptr);
typedef px_void(*pfun_PX_CanvasVMOnEnd)(px_surface* presult_surface, px_surface* ppreview_psurface, PX_CanvasNode path[],px_int path_node_count, px_void* ptr);

typedef struct 
{
	px_char name[PX_CANVASVM_MAX_TOOL_NAME];
	pfun_PX_CanvasVMOnBegin begin;
	pfun_PX_CanvasVMOnMove move;
	pfun_PX_CanvasVMOnEnd end;
	px_void* ptr;
}PX_CanvasVMTool;

typedef struct 
{
	px_memorypool* mp;
	px_surface* ppreview_surface,*presult_surface;
	PX_CanvasVMTool tools[PX_CANVASVM_MAX_TOOLS_COUNT];
	PX_CanvasNode currentPath[PX_CANVASVM_MAX_COUNT];
	px_int currentPathCount;
	px_memory payload;
	px_vector payload_offsets;
}PX_CanvasVM;

px_bool PX_CanvasVMInitialize(px_memorypool* mp, PX_CanvasVM *pCanvas);
px_void PX_CanvasVMOnBegin(PX_CanvasVM* pCanvas, px_float x, px_float y, px_float z);
px_void PX_CanvasVMOnMove(PX_CanvasVM* pCanvas, px_float x, px_float y, px_float z);
px_void PX_CanvasVMOnEnd(PX_CanvasVM* pCanvas);
px_void PX_CanvasVMSetResultTexture(PX_CanvasVM* pCanvas, px_texture* ptex);
px_void PX_CanvasVMSetPreviewTexture(PX_CanvasVM* pCanvas,px_texture *ptex);
#endif