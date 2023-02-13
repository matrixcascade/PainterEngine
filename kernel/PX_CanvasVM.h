#ifndef PX_CANVASVM_H
#define PX_CANVASVM_H
#include "../core/PX_Core.h"

#define PX_CANVASVM_MAX_COUNT 1024
#define PX_CANVASVM_MAX_TOOLS_COUNT 64
#define PX_CANVASVM_MAX_TOOL_NAME 32
#define PX_CANVASVM_MAX_DETAIL_SIZE PX_CANVASVM_MAX_COUNT

typedef enum 
{
	PX_CANVASVM_OPCODE_NONE,
	PX_CANVASVM_OPCODE_PEN,
	PX_CANVASVM_OPCODE_ERASER,
	PX_CANVASVM_OPCODE_SPRAY,
	PX_CANVASVM_OPCODE_PAINT,
	PX_CANVASVM_OPCODE_HAND,
}PX_CANVASVM_OPCODE;


typedef struct 
{
	px_dword opcode;
	px_float psize;
	px_dword size;
}PX_CanvasNode_Header;

typedef struct
{
	px_float32 x, y, z;
}PX_CanvasNode;
typedef struct _PX_CanvasVM PX_CanvasVM;


typedef px_void(*pfun_PX_CanvasVMOnPaint)(PX_CanvasVM* pCanvas, px_void* ptr);

typedef struct 
{
	px_char name[PX_CANVASVM_MAX_TOOL_NAME];
	px_dword opcode;
	pfun_PX_CanvasVMOnPaint paint;
	px_void* ptr;
}PX_CanvasVMTool;

typedef enum
{
	PX_CanvasVM_State_Standby,
	PX_CanvasVM_State_Painting,
}PX_CanvasVM_State;

typedef enum
{
	PX_CanvasVM_FilterMode_None,
	PX_CanvasVM_FilterMode_FT,
	PX_CanvasVM_FilterMode_Liner,
	PX_CanvasVM_FilterMode_BSpline,
	PX_CanvasVM_FilterMode_Breseham,
	PX_CanvasVM_FilterMode_Bezier,
	PX_CanvasVM_FilterMode_PainterEngineFilter,
}PX_CanvasVM_FilterMode;

typedef struct _PX_CanvasVM
{
	px_memorypool* mp;
	px_surface* ptarget_surface;
	px_surface* pmask_surface;
	PX_CanvasVMTool tools[PX_CANVASVM_MAX_TOOLS_COUNT];
	PX_CanvasNode currentPath[PX_CANVASVM_MAX_COUNT];
	px_int currentPathCount;
	px_memory payload;
	px_vector payload_offsets;
	px_int reg_tools_count;
	px_int reg_current_tool_index;
	px_dword reg_opcode;
	px_float reg_size;
	px_float reg_step;
	px_color reg_color;
	px_float reg_filterfactor;
	px_bool reg_showdot;
	px_color reg_dotcolor;
	PX_CanvasNode reg_lastpoint;
	PX_CanvasVM_FilterMode reg_filtermode;
	PX_CanvasVM_State reg_state;
}PX_CanvasVM;

px_bool PX_CanvasVMInitialize(px_memorypool* mp, PX_CanvasVM *pCanvas);
px_void PX_CanvasVMOnBegin(PX_CanvasVM* pCanvas, PX_CANVASVM_OPCODE opcode, px_float size, px_color color, px_float step, px_float reg_factor);
px_void PX_CanvasVMOnMove(PX_CanvasVM* pCanvas, px_float x, px_float y, px_float z);
px_void PX_CanvasVMPaint(PX_CanvasVM* pCanvas, px_surface *prender,px_surface *pmask);
px_void PX_CanvasVMOnEnd(PX_CanvasVM* pCanvas);
px_void PX_CanvasVMSetTargetTexture(PX_CanvasVM* pCanvas, px_texture* ptex);
px_void PX_CanvasVMSetMaskTexture(PX_CanvasVM* pCanvas, px_texture* ptex);
px_void PX_CanvasVMStorePayload(PX_CanvasVM* pCanvas);
px_void PX_CanvasVMRegisterTool(PX_CanvasVM* pCanvas, px_char name[PX_CANVASVM_MAX_TOOL_NAME], px_dword opcode, pfun_PX_CanvasVMOnPaint paint, px_void* ptr);
px_void PX_CanvasVMFree(PX_CanvasVM* pCanvas);
px_void PX_CanvasVMSetMode(PX_CanvasVM* pCanvas, PX_CanvasVM_FilterMode fmode);

px_void PX_CanvasVMFilter(PX_CanvasVM* pCanvas, PX_CanvasVM_FilterMode mode, px_int index, px_point samples[PX_CANVASVM_MAX_DETAIL_SIZE], px_int detail);
px_bool PX_CanvasVMSelectToolByIndex(PX_CanvasVM* pCanvas, px_int index);
px_bool PX_CanvasVMSelectToolByName(PX_CanvasVM* pCanvas, const px_char name[]);
px_void PX_CanvasVMSetSize(PX_CanvasVM* pCanvas, px_float size);
px_void PX_CanvasVMPushNode(PX_CanvasVM* pCanvas, PX_CanvasNode* pnode);
#endif