#ifndef PX_UI_H
#define PX_UI_H

#include "PX_Json.h"
#include "PX_Object.h"

typedef struct
{
	px_memorypool *mp,*ui_mp;
	px_map ObjectMap;
	px_vector infos;
	PX_FontModule *fontmodule;
}PX_UI;


typedef PX_Object * (*PX_UI_ControllerCreate)(PX_UI *ui,PX_Object *parent,PX_Json_Value *value,px_int width,px_int height);

typedef struct  
{
	px_char Type[32];
	PX_UI_ControllerCreate create_func;
}PX_UI_ControllerInfo;

px_bool PX_UIInitialize(px_memorypool *mp,px_memorypool *ui_mp,PX_UI *ui,PX_FontModule *fontmodule);
px_bool PX_UIAddControllerInfo(PX_UI *ui,const px_char controllertype[],PX_UI_ControllerCreate _func);
PX_Object * PX_UICreate(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height);
px_void PX_UIUpdateObjectsPostions(PX_UI *ui,PX_Object *update,PX_Json_Value *json_value,px_int width,px_int height);
PX_Object *PX_UIGetObjectByID(PX_UI *ui,const px_char id[]);
px_void PX_UIFree(PX_UI *ui);

#endif