#ifndef PX_DESIGNER_H
#define PX_DESIGNER_H

#include "../kernel/PX_Kernel.h"

#define PX_DESIGNER_NAME_LENGTH 48
#define PX_DESIGNER_MAX_PROPERTYS 32
#define PX_DESIGNER_CONTROLLER_ITEM_HEIGHT 20
typedef enum
{
	PX_DESIGNER_OBJECT_TYPE_UI,
	PX_DESIGNER_OBJECT_TYPE_GAME,
	PX_DESIGNER_OBJECT_TYPE_FUNCTION,
}PX_DESIGNER_OBJECT_TYPE;


typedef PX_Object *(*px_designer_createfunc)(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm);

typedef px_float (*px_designer_getproperty_float)(PX_Object *pObject);
typedef px_int (*px_designer_getproperty_int)(PX_Object *pObject);
typedef px_bool (*px_designer_getproperty_string)(PX_Object *pObject,px_string *str);
typedef px_bool (*px_designer_getproperty_bool)(PX_Object *pObject);

typedef px_void (*px_designer_setproperty_float)(PX_Object *pObject,px_float v);
typedef px_void (*px_designer_setproperty_int)(PX_Object *pObject,px_int v);
typedef px_void (*px_designer_setproperty_string)(PX_Object *pObject,const px_char v[]);
typedef px_void (*px_designer_setproperty_bool)(PX_Object *pObject,px_bool v);

typedef struct
{
	px_char Name[PX_DESIGNER_NAME_LENGTH];
	//get
	px_designer_getproperty_float getfloat;
	px_designer_getproperty_int getint;
	px_designer_getproperty_string getstring;
	px_designer_getproperty_bool getbool;

	//set
	px_designer_setproperty_float setfloat;
	px_designer_setproperty_int setint;
	px_designer_setproperty_string setstring;
	px_designer_setproperty_bool setbool;

}PX_Designer_Object_property;



typedef struct  
{
	px_char Name[PX_DESIGNER_NAME_LENGTH];
	px_designer_createfunc createfunc;
	PX_DESIGNER_OBJECT_TYPE type;
	PX_Designer_Object_property properties[PX_DESIGNER_MAX_PROPERTYS];
}PX_Designer_ObjectDesc;


typedef struct
{
	PX_Object *pObject;
	px_int descIndex;
	PX_DESIGNER_OBJECT_TYPE type;
}PX_Designer_Object;


typedef struct  
{
	px_memorypool *mp,*mp_ui,*mp_game;
	PX_Object *pLinkObject;
	PX_World  *pLinkWorld;

	PX_Object	*widget_controllers;
	PX_Object	*widget_property,*widget_propertyscrollarea;
	PX_Object* world_hscroll, *world_vscroll;
	PX_Object*	designerbox;
	PX_Object*	list_ObjectDesc;
	PX_Object*  button_delete;
	PX_Object*	messagebox;
	PX_Object*	menu,*menu_run,*menu_exit,menu_window_controllers,menu_window_properties;
	PX_Object*  label_propertys[PX_DESIGNER_MAX_PROPERTYS]; 
	PX_Object*  edit_propertys[PX_DESIGNER_MAX_PROPERTYS]; 
	PX_FontModule *fm;
	px_int allocID;
	px_int selectObjectIndex;
	px_bool showsliderbar;
	px_float lastcursorx;
	px_float lastcursory;
	px_vector ObjectDesc;
	px_vector Objects;
}PX_Designer;

PX_Object * PX_DesignerCreate(px_memorypool *mp,px_memorypool *mp_ui,px_memorypool*mp_world,PX_Object *pparent,PX_Object *pLinkObject,PX_World *pLinkWorld,PX_FontModule *fm);
px_void PX_DesignerActivate(PX_Object* pObject);
px_void PX_DesignerClose(PX_Object* pObject);
px_bool PX_DesignerExport(PX_Object* pObject, px_string* pText);
px_bool PX_DesignerImport(PX_Object* pObject, const px_char* pText);
#endif