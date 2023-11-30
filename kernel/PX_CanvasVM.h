#ifndef PX_CANVASVM_H
#define PX_CANVASVM_H
#include "../core/PX_Core.h"

#define PX_CANVASVM_MAX_COUNT 1024
#define PX_CANVASVM_MAX_TOOLS_COUNT 64
#define PX_CANVASVM_MAX_TOOL_NAME 32
#define PX_CANVASVM_MAX_DETAIL_SIZE PX_CANVASVM_MAX_COUNT
#define PX_CANVASVM_STEP_DIV 8
#define PX_CANVASVM_LAYER_MAX 8
#define PX_CANVASVM_LAYER_NAME_LENGTH 64

typedef enum 
{
	PX_CANVASVM_OPCODE_NONE,
	PX_CANVASVM_OPCODE_PAINT,
	PX_CANVASVM_OPCODE_LAYEREDIT,
	PX_CANVASVM_OPCODE_LAYERMOVEDOWN,
	PX_CANVASVM_OPCODE_LAYERMOVEUP,
	PX_CANVASVM_OPCODE_LAYERDELETE,
	PX_CANVASVM_OPCODE_LAYERCREATE,
	PX_CANVASVM_OPCODE_LAYERVISIBLE,
	PX_CANVASVM_OPCODE_LAYERCLIP,
}PX_CANVASVM_OPCODE;


typedef struct 
{
	px_dword opcode;
	px_dword size;
	px_dword payload;
	px_float psize;
	px_float filter;
	px_color color;
}PX_CanvasVMShell_Header;

typedef struct
{
	px_float32 x, y, z;
}PX_CanvasNode;
typedef struct _PX_CanvasVM PX_CanvasVM;


typedef px_void(*pfun_PX_CanvasVMOnPaint)(px_surface* psurface,px_float x, px_float y,px_float size,px_color color, px_void* ptr);

typedef struct 
{
	px_char name[PX_CANVASVM_MAX_TOOL_NAME];
	pfun_PX_CanvasVMOnPaint paint;
	px_bool store;
	px_void* ptr;
}PX_CanvasVMTool;

typedef enum
{
	PX_CanvasVM_State_Standby,
	PX_CanvasVM_State_Painting,
}PX_CanvasVM_State;

typedef enum
{
	PX_OBJECT_LAYER_ATTRIBUTE_NORMAL,
	PX_OBJECT_LAYER_ATTRIBUTE_CLIP,
}PX_OBJECT_LAYER_ATTRIBUTE;



typedef struct
{
	px_bool activating;
	px_char name[PX_CANVASVM_LAYER_NAME_LENGTH];
	px_texture surface_preview_mini, surface_layer;
	px_bool editing;
	px_bool visible;
	PX_OBJECT_LAYER_ATTRIBUTE attribute;
}PX_Object_Layer;



typedef struct _PX_CanvasVM
{
	px_memorypool* mp;
	PX_CanvasVMTool tools[PX_CANVASVM_MAX_TOOLS_COUNT];
	PX_CanvasNode currentPath[PX_CANVASVM_MAX_COUNT];
	px_int currentPathCount;
	
	px_int   reg_tools_count;

	px_int   reg_tool, reg_tool2;
	px_float reg_size, reg_size2;
	px_color reg_color, reg_color2;
	px_float reg_filter_radius, reg_filter_radius2;



	px_bool  reg_showdot;
	px_color reg_dotcolor;
	px_point reg_lastCursorPos;
	
	
	PX_CanvasVM_State reg_state;
	px_bool reg_r_drawfront, reg_r_drawback;
	px_int   width, height;
	px_float view_x, view_y;
	px_float scale, min_scale, max_scale;
	
	px_texture cache_surface, cache_backward_surface, *pcache_editing_surface, cache_frontward_surface, *pcache_mask_surface;
	px_texture view_surface;

	px_int	 reg_ip;
	px_memory shell;
	PX_Object_Layer layers[PX_CANVASVM_LAYER_MAX];
	
	px_int	 reg_snapshot_ip;
	PX_Object_Layer snapshot_layers[PX_CANVASVM_LAYER_MAX];


	px_int   reg_state_id;
}PX_CanvasVM;

px_bool PX_CanvasVMInitialize(px_memorypool* mp, PX_CanvasVM *pCanvasVM, px_int width, px_int height, px_int zoom_width, px_int zoom_height);
px_void PX_CanvasVMSetZoom(PX_CanvasVM* pCanvasVM, px_int zoom_width, px_int zoom_height);
px_void PX_CanvasVMOnBegin(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMOnMove(PX_CanvasVM* pCanvasVM, px_float x, px_float y, px_float z);
px_void PX_CanvasVMOnEnd(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMStorePaintShell(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMRegisterTool(PX_CanvasVM* pCanvasVM, const px_char name[PX_CANVASVM_MAX_TOOL_NAME], pfun_PX_CanvasVMOnPaint paint, px_bool store, px_void* ptr);
px_void PX_CanvasVMFree(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMReset(PX_CanvasVM* pCanvasVM);
px_bool PX_CanvasVMExport(PX_CanvasVM* pCanvasVM,px_memory *bin);
px_bool PX_CanvasVMExportAsPng(PX_CanvasVM* pCanvasVM, px_memory* bin);
px_bool PX_CanvasVMImport(PX_CanvasVM* pCanvasVM,const px_byte data[],px_int size);
px_void PX_CanvasVMPushad(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMPopad(PX_CanvasVM* pCanvasVM);
px_int  PX_CanvasVMGetEditingLayerIndex(PX_CanvasVM* pCanvasVM);

px_bool PX_CanvasVMSelectToolByName(PX_CanvasVM* pCanvasVM, const px_char name[]);
px_void PX_CanvasVMSetSize(PX_CanvasVM* pCanvasVM, px_float size);
px_void PX_CanvasVMSetColor(PX_CanvasVM* pCanvasVM, px_color color);
px_void PX_CanvasVMSetFilter(PX_CanvasVM* pCanvasVM, px_float filter);
px_void PX_CanvasVMSetViewPosition(PX_CanvasVM* pCanvasVM, px_float x, px_float y);
px_void PX_CanvasVMSetViewScale(PX_CanvasVM* pCanvasVM, px_float scale);
px_void PX_CanvasVMResetView(PX_CanvasVM *pCanvasVM);


px_void PX_CanvasVMRepaintLayerMiniPreview(PX_CanvasVM* pCanvasVM, px_int layerindex);
px_void PX_CanvasVMRepaintEditingLayerMiniPreview(PX_CanvasVM* pCanvasVM);


px_void PX_CanvasVMLayerSetEditing(PX_CanvasVM* pCanvasVM, px_int index);
px_void PX_CanvasVMLayerCreate(PX_CanvasVM* pCanvasVM, const px_char name[]);
px_void PX_CanvasVMLayerDelete(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMLayerMoveUp(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMLayerMoveDown(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMLayerVisible(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMLayerClip(PX_CanvasVM* pCanvasVM);


px_bool PX_CanvasVMExecuteShell(PX_CanvasVM* pCanvasVM);

px_void PX_CanvasVMRepaintBackFrontCache(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMRepaintCache(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMRepaintCacheToView(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMRepaintAllCache(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMRepaintAllLayersPreview(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMUpdateMaskEditingCache(PX_CanvasVM* pdesc);
px_point2D PX_CanvasVMCanvasPostionToViewPosition(PX_CanvasVM* pCanvasVM, px_point2D canvasPos);
px_point2D PX_CanvasVMViewPositionToCanvasPostion(PX_CanvasVM* pdesc, px_point2D ViewPos);

px_void PX_CanvasVMMoveForward(PX_CanvasVM* pCanvasVM);
px_void PX_CanvasVMMoveBack(PX_CanvasVM* pCanvasVM);
px_int  PX_CanvasVMGetLastShellIP(PX_CanvasVM* pCanvasVM);

#endif