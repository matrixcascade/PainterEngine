#ifndef PX_DESIGNER_H
#define PX_DESIGNER_H

#include "../kernel/PX_Kernel.h"

#define PX_DESIGNER_NAME_LENGTH 48
#define PX_DESIGNER_MAX_PROPERTYS 32
#define PX_DESIGNER_CONTROLLER_ITEM_HEIGHT 20
typedef enum
{
	PX_DESIGNER_CONTROLLER_STATIC,
	PX_DESIGNER_CONTROLLER_DYNAMIC,
}PX_DESIGNER_CONTROLLER_TYPE;

typedef enum
{
	PX_Designer_Controller_Property_Type_Float,
	PX_Designer_Controller_Property_Type_Int,
	PX_Designer_Controller_Property_Type_String,
	PX_Designer_Controller_Property_Type_Bool,
}PX_Designer_Controller_Property_Type;


typedef PX_Object *(*px_designer_createfunc)(px_memorypool *mp,px_float x,px_float y,px_float width,px_float height);
typedef PX_Object *(*px_designer_getproperty)(PX_Object *pObject);
typedef PX_Object *(*px_designer_setproperty)(PX_Object *pObject);
typedef struct
{
	px_char Name[PX_DESIGNER_NAME_LENGTH];
	px_designer_getproperty func_getproperty;
	px_designer_setproperty func_setproperty;
	PX_Designer_Controller_Property_Type type;
}PX_Designer_Controller_property;



typedef struct  
{
	px_char Name[PX_DESIGNER_NAME_LENGTH];
	PX_Object *pObject;
	px_designer_createfunc createfunc;
	PX_DESIGNER_CONTROLLER_TYPE type;
	PX_Designer_Controller_property properties[PX_DESIGNER_MAX_PROPERTYS];
}PX_Designer_ControllerType;

typedef enum 
{
	PX_DESIGNER_MODE_STATIC=1,
	PX_DESIGNER_MODE_DYNAMIC=2,
	PX_DESIGNER_MODE_ALL=3,
}PX_DESIGNER_MODE;

typedef struct  
{
	PX_World	world;
	PX_Object	*static_root;
	PX_Object	*widget_controllers;
	PX_Object	*widget_property;
	PX_Object* panc_designerbox;
	PX_Object* list_controllers;
	PX_Object* messagebox;
	PX_Object *menu;
	PX_FontModule *fm;
	PX_Object *selectcontroller;
	px_vector controllersDesc;
	PX_DESIGNER_MODE mode;
}PX_Designer;

PX_Object * PX_DesignerCreate(px_memorypool *mp,PX_Object *pparent,px_int surface_width,int surface_height,int world_height,px_int world_width,PX_FontModule *fm, PX_DESIGNER_MODE mode);
#endif