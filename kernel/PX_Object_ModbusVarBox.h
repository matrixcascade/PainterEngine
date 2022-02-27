#ifndef PX_OBJECT_MODBUSVARBOX_H
#define PX_OBJECT_MODBUSVARBOX_H
#include "PX_Object.h"


typedef struct
{
	PX_Object	*widget;
	PX_Object* various_name;
	PX_Object* various_address;
	PX_Object* selectbar_type;
	PX_Object* various_value;
	PX_Object* checkbox_readonly;
	PX_Object	*btn_close;
	PX_Object   *btn_ok;
	px_bool   EditMode;
}PX_Object_ModbusVarBox;

PX_Object_ModbusVarBox* PX_Object_GetModbusVarBox( PX_Object *Object );
PX_Object * PX_Object_ModbusVarBoxCreate(px_memorypool *mp,PX_Object *Parent,int x,int y,int width,int height,const px_char title[],PX_FontModule *fontmodule);
px_void PX_Object_ModbusVarBoxShow(PX_Object *pObject);
px_void PX_Object_ModbusVarBoxShowEdit(PX_Object* pObject, const px_char name[], px_dword address, px_int type, px_word value);
px_void PX_Object_ModbusVarBoxClose(PX_Object* pObject);
#endif

