#ifndef PX_MODULE_SERIALPORT_H
#define PX_MODULE_SERIALPORT_H

#include "PX_Object.h"

#define PX_INSTANCE_SERIALPORTCONFIGURARION_NEXT 0x1000

typedef struct  
{
	px_char com[16];
	px_uint baudRate;
	px_uint DataBits;/*5,6,7,8*/
	px_char ParityType;/*'O','E','N'*/
	px_int  stopBit;/*0,1,2*/
}PX_Object_COMBox_Return;

typedef struct
{
	PX_Object *widget;
	PX_FontModule *fm;
	PX_Object *various_com,* various_baudrate,* various_databits,* various_stopbits,* various_parity;
	PX_Object *btn_ok,*btn_close;
}PX_Object_COMBox;

PX_Object_COMBox* PX_Object_GetCOMBox(PX_Object* pObject);
PX_Object* PX_Object_COMBoxCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, const px_char title[], PX_FontModule* fontmodule);
px_void PX_Object_COMBoxShow(PX_Object* pObject);
px_void PX_Object_COMBoxClose(PX_Object* pObject);
PX_Object_COMBox_Return PX_Object_COMBoxGetReturn(PX_Object* pObject);

typedef struct
{
	PX_Object* widget;
	PX_Object* various_ip;
	PX_Object* various_port;
	PX_Object* btn_close;
	PX_Object* btn_ok;
}PX_Object_IPBox;

PX_Object_IPBox* PX_Object_GetIPBox(PX_Object* pObject);
PX_Object* PX_Object_IPBoxCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, const px_char title[], PX_FontModule* fontmodule);
px_void PX_Object_IPBoxShow(PX_Object* pObject);
px_void PX_Object_IPBoxClose(PX_Object* pObject);
px_void PX_Object_IPBoxSetDefault(PX_Object* pObject, const px_char ip[], const px_char port[]);


#include "PX_CanvasVM.h"

typedef enum
{
	PX_OBJECT_PAINTERBOX_TOOL_TYPE_NONE,
	PX_OBJECT_PAINTERBOX_TOOL_TYPE_PEN,
	PX_OBJECT_PAINTERBOX_TOOL_TYPE_ERASER,
	PX_OBJECT_PAINTERBOX_TOOL_TYPE_SPRAY,
	PX_OBJECT_PAINTERBOX_TOOL_TYPE_PAINT,
	PX_OBJECT_PAINTERBOX_TOOL_TYPE_HAND,
}PX_OBJECT_PAINTERBOX_TOOL_TYPE;


typedef struct
{
	PX_Object* widget;
	PX_Object* color_panel, * button_pen, * button_paint, * button_eraser, * button_spray, * button_hand, * button_back, * button_forward, * button_viewreset;
	PX_Object* sliderbar_size, * sliderbar_alpha, * sliderbar_smooth, * image_present;
	px_texture tex_Eraser, tex_Paint, tex_Pen, tex_Spray, tex_Hand, tex_back, tex_forward, tex_viewreset;
	px_texture present;
	PX_CanvasVM* pCanvasVM;
	PX_OBJECT_PAINTERBOX_TOOL_TYPE tool_type;
}PX_Object_PainterBox;

px_float PX_Object_PainterBoxGetToolSize(PX_Object* pObject);
px_float PX_Object_PainterBoxGetToolAlpha(PX_Object* pObject);
px_color PX_Object_PainterBoxGetToolColor(PX_Object* pObject);
px_float PX_Object_PainterBoxGetToolSmooth(PX_Object* pObject);
PX_Object* PX_Object_PainterBoxCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, PX_CanvasVM* pCanvasVM);
PX_OBJECT_PAINTERBOX_TOOL_TYPE PX_Object_PainterBoxGetCurrentTool(PX_Object* pObject);

typedef struct
{
	px_memorypool* ui_mp;
	PX_Object* widget;
	PX_Object* list, * image;
	PX_FontModule* fm;
	px_texture tex_view;
	px_texture tex_clip, tex_new, tex_delete, tex_visible, tex_invisible, tex_edit, tex_down, tex_up;
	PX_Object* btn_new, * btn_delete, * btn_up, * btn_down, * btn_clip;
	px_int reg_state_id;
	PX_CanvasVM* pCanvasVM;
}PX_Object_LayerBox;

PX_Object* PX_Object_LayerBoxCreate(px_memorypool* ui_mp, PX_Object* Parent, px_int x, px_int y, PX_FontModule* fm, PX_CanvasVM* pCanvasVM);
px_texture* PX_Object_LayerBoxGetPreviewSurface(PX_Object* pObject);

#endif