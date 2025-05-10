#ifndef PX_CDA_H
#define PX_CDA_H

#include "../core/PX_Core.h"
#include "PX_VM.h"
#include "PX_Resource.h"
#include "PX_Compiler.h"
#include "PX_Object.h"

typedef enum
{
	PX_CDA_GRID_STATE_IDLE,
	PX_CDA_GRID_STATE_CURSOR,
}PX_CDA_GRID_STATE;

#define PX_CDA_GRID_DEFAULE_SIZE 16
#define PX_CDA_PROPERTIES_MAX_COUNT 32

typedef struct
{
	px_variable var;
	px_dword timestamp;
	PX_CDA_GRID_STATE state;
}PX_CDA_Grid;

typedef enum
{
	PX_OBJECT_CDA_DISPLAY_TYPE_TEXTURE,
	PX_OBJECT_CDA_DISPLAY_TYPE_GIF,
	PX_OBJECT_CDA_DISPLAY_TYPE_2DX,
	PX_OBJECT_CDA_DISPLAY_TYPE_STRING,
	PX_OBJECT_CDA_DISPLAY_TYPE_NONE,
}PX_OBJECT_CDA_DISPLAY_TYPE;

typedef enum
{
	PX_CDA_ROUTE_DIRECTION_TWO_WAY,
	PX_CDA_ROUTE_DIRECTION_ONE_WAY,
	PX_CDA_ROUTE_DIRECTION_NONE,
}PX_CDA_ROUTE_DIRECTION;

typedef struct _PX_CDA PX_CDA;
typedef struct _PX_CDA_Object PX_CDA_Object;
typedef PX_Object* (*__px_cda_createobject_callback)(PX_Object *pObject);
#define PX_CDA_CREATE_OBJECT_CALLBACK_FUNCTION(name) PX_Object * name(PX_Object *pObject)
typedef px_void (*__px_cda_object_property_change_callback)(PX_Object* pObject, px_int index);
#define PX_CDA_OBJECT_PROPERTY_CHANGE_CALLBACK_FUNCTION(name) px_void name(PX_Object* pObject, px_int index)

typedef enum
{
	PX_CDA_OBJECT_PROPERTY_TYPE_STRING,
	PX_CDA_OBJECT_PROPERTY_TYPE_INT,
	PX_CDA_OBJECT_PROPERTY_TYPE_FLOAT,
	PX_CDA_OBJECT_PROPERTY_TYPE_BOOL,
}PX_CDA_OBJECT_PROPERTY_TYPE;

#define PX_CDA_OBJECT_PROPERTIES_CONTENT_MAX 1024

typedef struct
{
	px_char name[PX_OBJECT_ID_MAXLEN];
	PX_CDA_OBJECT_PROPERTY_TYPE type;
	px_char defaults[PX_CDA_OBJECT_PROPERTIES_CONTENT_MAX];
	px_float max,min;
	__px_cda_object_property_change_callback func_callback;
}PX_CDA_Object_PropertiesDesc;

typedef struct
{
	px_char name[PX_OBJECT_ID_MAXLEN];
	px_char value[PX_CDA_OBJECT_PROPERTIES_CONTENT_MAX];
}PX_CDA_Object_Property;


typedef struct
{
	px_char name[PX_OBJECT_ID_MAXLEN];
	px_char group[PX_OBJECT_ID_MAXLEN];
	px_float linewidth;
	px_float busSpeed;
	px_color color;
	px_bool show_in_play;
	PX_FontModule* fontmodule;
	px_texture* pplace_texture;
	px_texture* pico_texture;

	PX_CDA_ROUTE_DIRECTION direction;
}PX_CDA_RouteClass;

typedef enum
{
	PX_CDA_SIGNAL_DIRECTION_FORWARD=0,
	PX_CDA_SIGNAL_DIRECTION_BACKWARD,
}PX_CDA_SIGNAL_DIRECTION;

typedef struct
{
	px_variable var;
	PX_CDA_SIGNAL_DIRECTION direction;
	px_dword timestamp;
	px_float x;
}PX_CDA_Signal;

typedef struct
{
	px_byte direction;
	px_byte type;
	px_word length;
}PX_CDA_RouteLine;

typedef enum
{
	PX_CDA_ROUTE_MODE_NORMAL=0,
	PX_CDA_ROUTE_MODE_CURSOR,
	PX_CDA_ROUTE_MODE_SELECT,
}PX_CDA_ROUTE_MODE;


typedef struct
{
	px_int start_grid_x, start_grid_y;
	px_int end_grid_x, end_grid_y;
	px_float length;
	PX_CDA_ROUTE_MODE mode;
	PX_CDA_RouteClass *prouteClass;
	px_dword start_timestamp, end_timestamp;
	px_dword last_signal_time;
	px_vector routes;//PX_CDA_ROUTE
	px_vector signals;//PX_CDA_Signal
}PX_CDA_Route;

typedef struct
{
	px_char		name[PX_OBJECT_ID_MAXLEN];
	px_char     group[PX_OBJECT_ID_MAXLEN];
	px_int		grid_width, grid_height;
	px_bool     place_align;
	px_bool     static_scale;
	PX_ALIGN    render_align;
	PX_FontModule* fontmodule;
	px_texture* pplace_texture;
	px_texture* pico_texture;
	px_color fontcolor;
	PX_OBJECT_CDA_DISPLAY_TYPE display_type;
	px_memory vmbin;
	PX_VM_DebuggerMap debugmap;
	__px_cda_createobject_callback createObjectCallback;
	px_void* callback_user_ptr;
	px_int alloc_id;
	PX_CDA_Object_PropertiesDesc properties[PX_CDA_PROPERTIES_MAX_COUNT];
	px_char** port_desciptions;
}PX_CDA_ObjectClass;

typedef enum
{
	PX_CDA_OBJECT_MODE_NORMAL=0,
	PX_CDA_OBJECT_MODE_CURSOR,
	PX_CDA_OBJECT_MODE_SELECT,
}PX_CDA_OBJECT_MODE;




typedef struct _PX_CDA_Object
{
	px_void* pCDA;

	px_int	  cda_index;
	px_float  grid_x, grid_y;
	px_int    grid_width, grid_height;
	px_dword  *grid_lasttimestamp;
	PX_CDA_OBJECT_MODE mode;
	PX_CDA_Object_Property properties[PX_CDA_PROPERTIES_MAX_COUNT];
	//display present
	PX_CDA_ObjectClass *pObjectClass;

	union
	{
		px_texture* pTexture;
		px_gif gif;
		PX_Animation animation;
		px_string text;
	};

	//vm
	px_uint64 rand_seed;
	PX_VM   vm;
}PX_CDA_Object;



typedef struct _PX_CDA
{
	px_memorypool* mp;
	px_memorypool* mp_static;
	//view
	px_float   cursor_grid_x, cursor_grid_y;
	px_float   camera_x, camera_y;
	px_float   camera_scale;
	px_int	   view_width,view_height;
	px_int     grid_size;
	
	px_color   grid_line_color;
	px_bool    show_grid_line;
	px_bool    show_controller_size;
	px_bool    show_id_text;
	px_bool    show_objects;
	px_bool    show_grid_info;


	//grid
	px_int     grid_x_count,grid_y_count;
	PX_CDA_Grid *pgrids;
	px_dword   grid_timestamp;
	
	//object manager
	px_vector pObjects;//PX_CDA_Object;
	px_vector objects_group;

	//route manage
	px_int editing_route_pin;
	PX_CDA_Route editing_route;
	px_vector routes;
	px_vector routes_group;

	//render
	px_texture *psource_background_texture, background_texture;
	px_texture snapshot_texture;

	//payload
	px_int ip;
	px_memory payload;


	//lib
	PX_FontModule* fontmodule;
	PX_ResourceLibrary *presourceLibrary;
	PX_SoundPlay *soundplay;
	PX_VM_DebuggerMap debugmap;
	PX_VM  vm;
	PX_MT19937 mt19937;
	px_map objectClasses,routeClasses;

	//last 
	PX_CDA_ObjectClass *pLastCreatedObjectClass;
	px_bool last_opcode_is_set_property;
	px_int	last_set_property_object_index;
	px_int	last_set_property_index;
}PX_CDA;

PX_CDA_Object* PX_Object_GetCDAObject(PX_Object* pObject);

px_bool PX_CDA_AddObjectClass(PX_CDA* pCDA, PX_CDA_ObjectClass* pclass);

px_bool PX_CDA_AddRouteClass(PX_CDA* pCDA, PX_CDA_RouteClass* pclass);

px_void PX_CDA_Object_ID_PropertyChangedCallback(PX_Object* pObject, px_int index);

px_bool PX_CDA_AddObject(PX_CDA* pCDA, PX_Object* pObject);

PX_Object* PX_CDA_CreateClassObject(PX_CDA* pCDA, px_char classname[], px_float grid_x, px_float grid_y);

px_void PX_CDA_RemoveObject(PX_CDA* pCDA, PX_Object* pObject);

px_void PX_CDA_SetBackwardTexture(PX_CDA* pCDA, px_texture* ptexture);

px_void PX_CDA_SetViewGridSize(PX_CDA* pCDA, px_int size);

px_void PX_CDA_ShowGridLine(PX_CDA* pw, px_bool bline);

PX_Object* PX_CDA_GetCDAObjectByID(PX_CDA* CDA, const px_char id[]);

px_int PX_CDA_GetObjectCount(PX_CDA* CDA);

px_int PX_CDA_GetAliveCount(PX_CDA* CDA);

px_texture* PX_CDA_TakeSnapShot(PX_CDA* pCDA);

px_bool PX_CDA_BeginRouteEdit(PX_CDA* pCDA, px_int start_grid_x, px_int start_grid_y, PX_CDA_RouteClass* pRouteClass);

px_bool PX_CDA_RouteMoving(PX_CDA* pCDA,px_int grid_x,px_int grid_y );

px_void PX_CDA_MoveSelectObject(PX_CDA* pCDA, px_float grid_x, px_float grid_y);

px_void PX_CDA_MoveSelectObjectLast(PX_CDA* pCDA, px_float grid_x, px_float grid_y);

px_bool PX_CDA_RoutePin(PX_CDA* pCDA);

px_bool PX_CDA_EndRouteEdit(PX_CDA* pCDA,px_bool save);

px_void PX_CDA_SetPropertyIndex(PX_CDA* pCDA, px_int index, px_int propertyIndex, const px_char value[]);

px_bool PX_CDA_CancelRouteEdit(PX_CDA* pCDA);

px_bool PX_CDA_ObjectCompileScript( const px_char script[], px_memory* pvmbin, PX_VM_DebuggerMap* pdebugmap);

px_void PX_CDA_DeleteSelectObject(PX_CDA* pCDA);

px_void PX_CDA_DeleteRoute(PX_CDA* pCDA, px_int index);

px_void PX_CDA_DeleteSelectRoute(PX_CDA* pCDA);

px_bool PX_CDA_isInRouteEditing(PX_CDA* pCDA);

px_void PX_CDA_SelectClear(PX_CDA* pCDA);

px_bool PX_CDA_SelectObject(PX_CDA* pCDA, px_float grid_x,px_float grid_y);

px_bool PX_CDA_SelectRoute(PX_CDA* pCDA, px_float grid_x,px_float grid_y);

px_bool PX_CDA_HasSelectingRoute(PX_CDA* pCDA);

px_bool PX_CDA_HasSelectingObject(PX_CDA* pCDA);

PX_Object *PX_CDA_GetSelectObject(PX_CDA* pCDA);

px_void PX_CDA_Rollback(PX_CDA *pCDA);

px_void PX_CDA_Forward(PX_CDA* pCDA);

px_void PX_CDA_ClearForwardInstuctions(PX_CDA* pCDA);

px_void PX_CDA_ObjectSetPortVariable(PX_Object* pCDAObject, px_int port, px_variable* pvar);

px_variable *PX_CDA_ObjectGetPortVariable(PX_Object* pCDAObject, px_int port);

const px_char *PX_CDA_ObjectGetPropertyValue(PX_Object* pCDAObject, const px_char name[]);

const px_char* PX_CDA_ObjectGetPropertyValueIndex  (PX_Object* pCDAObject, px_int index);

px_dword PX_CDA_ObjectGetPortTimeStamp(PX_Object* pCDAObject, px_int port);

PX_CDA_Grid *PX_CDA_ObjectGetPortGrid(PX_Object* pCDAObject, px_int port);

PX_CDA_ObjectClass *PX_CDA_GetObjectClass(PX_CDA* pCDA,px_char name[]);

PX_CDA_ObjectClass* PX_CDA_GetLastCreatedObjectClass(PX_CDA* pCDA);

px_void PX_CDA_SetLastCreatedObjectClassPropertyCallback(PX_CDA* pCDA, px_char name[], __px_cda_object_property_change_callback func_callback);

px_bool PX_CDA_ObjectClassSetCreateCallback(PX_CDA* pCDA,px_char name[],__px_cda_createobject_callback callback,px_void* ptr);

px_bool	PX_CDA_Initialize(px_memorypool* mp, px_memorypool* mp_static, PX_CDA* pCDA, px_int grid_x_count, px_int grid_y_count, px_int view_width, px_int view_height, PX_FontModule* fm, PX_ResourceLibrary* plib,PX_SoundPlay *soundplay);

px_void PX_CDA_Update(PX_CDA* pCDA, px_uint elapsed);

px_void PX_CDA_Render(px_surface* psurface, PX_CDA* pCDA, px_dword elapsed);

px_void PX_CDA_Free(PX_CDA* pCDA);

px_void PX_CDA_OnViewZoomScale(PX_CDA* pCDA, px_bool large);

px_void PX_CDA_PostEvent(PX_CDA* pCDA, PX_Object_Event e);

const px_char *PX_CDA_QueryGridPortDescription(PX_CDA* pCDA, px_float gridx, px_float gridy);

PX_Object* PX_CDA_GetCDAObjectByIndex(PX_CDA* CDA, px_int index);
#endif


