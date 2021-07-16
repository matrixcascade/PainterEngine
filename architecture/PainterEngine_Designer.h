#ifndef PX_DESIGNER_H
#define PX_DESIGNER_H

#include "../kernel/PX_Kernel.h"

#define PX_DESIGNER_NAME_LENGTH 48
#define PX_DESIGNER_MAX_PROPERTYS 32
#define PX_DESIGNER_CONTROLLER_ITEM_HEIGHT 20
typedef enum
{
	PX_DESIGNER_OBJECT_TYPE_STATIC,
	PX_DESIGNER_OBJECT_TYPE_DYNAMIC,
}PX_DESIGNER_OBJECT_TYPE;


typedef PX_Object *(*px_designer_createfunc)(px_memorypool *mp,PX_Object *pparent,px_float x,px_float y,px_float width,px_float height,PX_FontModule *fm);

typedef px_float (*px_designer_getproperty_float)(PX_Object *pObject);
typedef px_int (*px_designer_getproperty_int)(PX_Object *pObject);
typedef const px_char *(*px_designer_getproperty_string)(PX_Object *pObject);
typedef px_bool (*px_designer_getproperty_bool)(PX_Object *pObject);

typedef px_void (*px_designer_setproperty_float)(PX_Object *pObject,px_float v);
typedef px_void (*px_designer_setproperty_int)(PX_Object *pObject,px_int v);
typedef px_void (*px_designer_setproperty_string)(PX_Object *pObject,const px_char v[]);
typedef px_void (*px_designer_setproperty_bool)(PX_Object *pObject,px_bool v);
typedef struct
{
	px_char Name[PX_DESIGNER_NAME_LENGTH];
	//get
	union
	{
		px_designer_getproperty_float getfloat;
		px_designer_getproperty_int getint;
		px_designer_getproperty_string getstring;
		px_designer_getproperty_bool getbool;
	};
	//set
	union
	{
		px_designer_setproperty_float setfloat;
		px_designer_setproperty_int setint;
		px_designer_setproperty_string setstring;
		px_designer_setproperty_bool setbool;
	};
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
	PX_DESIGNER_OBJECT_TYPE type;
}PX_Designer_Object;


typedef struct  
{
	px_memorypool *mp;
	PX_World	world;
	PX_Object	*static_root;
	PX_Object	*widget_controllers;
	PX_Object	*widget_property;
	PX_Object*	designerbox;
	PX_Object*	list_ObjectDesc;
	PX_Object*	messagebox;
	PX_Object*	menu;
	PX_FontModule *fm;
	px_int allocID;
	px_int selectObjectIndex;
	px_float lastcursorx;
	px_float lastcursory;
	px_vector ObjectDesc;
	px_vector Objects;
}PX_Designer;

PX_Object * PX_DesignerCreate(px_memorypool *mp,PX_Object *pparent,px_int surface_width,int surface_height,int world_height,px_int world_width,PX_FontModule *fm);
#endif