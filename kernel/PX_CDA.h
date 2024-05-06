#ifndef PX_CDA_H
#define PX_CDA_H

#include "../core/PX_Core.h"
#include "PX_Script_VM.h"
#include "PX_Resource.h"
#include "PX_Compiler.h"
#include "PX_Object.h"

typedef enum
{
	PX_CDA_GRID_STATE_IDLE,
	PX_CDA_GRID_STATE_CURSOR,
}PX_CDA_GRID_STATE;

#define PX_CDA_GRID_CONTAIN_OBJECT_MAX 16
#define PX_CDA_GRID_DEFAULE_SIZE 16

typedef struct
{
	PX_VM_VARIABLE var;
	px_dword timestamp;
	PX_CDA_GRID_STATE state;
}PX_CDA_Grid;

typedef enum
{
	PX_OBJECT_CDA_DISPLAY_TYPE_TEXTURE,
	PX_OBJECT_CDA_DISPLAY_TYPE_GIF,
	PX_OBJECT_CDA_DISPLAY_TYPE_2DX,
	PX_OBJECT_CDA_DISPLAY_TYPE_STRING,
}PX_OBJECT_CDA_DISPLAY_TYPE;

typedef enum
{
	PX_CDA_ROUTE_DIRECTION_TWO_WAY,
	PX_CDA_ROUTE_DIRECTION_ONE_WAY,
	PX_CDA_ROUTE_DIRECTION_NONE,
}PX_CDA_ROUTE_DIRECTION;
typedef struct
{
	px_char name[PX_OBJECT_ID_MAXLEN];
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
	PX_VM_VARIABLE var;
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
}PX_CDA_ObjectClass;

typedef enum
{
	PX_CDA_OBJECT_MODE_NORMAL=0,
	PX_CDA_OBJECT_MODE_CURSOR,
	PX_CDA_OBJECT_MODE_SELECT,
}PX_CDA_OBJECT_MODE;

typedef struct
{
	px_void* pCDA;

	px_float  grid_x, grid_y;
	px_int    grid_width, grid_height;
	px_bool   static_scale;
	PX_CDA_OBJECT_MODE mode;

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
	//view
	px_float   camera_x, camera_y;
	px_float   camera_scale;
	px_int	   view_width,view_height;
	px_int     grid_size;
	
	px_color   grid_line_color;
	px_bool    show_grid_line;
	px_bool    show_controller_size;
	px_bool    show_id_text;

	//grid
	px_int     grid_x_count,grid_y_count;
	PX_CDA_Grid *pgrids;
	
	//object manager
	px_vector pObjects;//PX_CDA_Object;

	//route manage
	px_int editing_route_pin;
	PX_CDA_Route editing_route;
	px_vector routes;

	//render
	px_texture *psource_background_texture, background_texture;
	px_texture snapshot_texture;

	//payload
	px_int ip;
	px_memory payload;


	//lib
	PX_FontModule* fontmodule;
	PX_ResourceLibrary *presourceLibrary;
	PX_VM_DebuggerMap debugmap;
	PX_VM  vm;
	px_uint64 rand_seed;
	px_uint32 id_alloc;
	px_map objectClasses,routeClasses;
}PX_CDA;


px_bool PX_CDA_AddObjectClass(PX_CDA* pCDA, PX_CDA_ObjectClass* pclass);

px_bool PX_CDA_AddRouteClass(PX_CDA* pCDA, PX_CDA_RouteClass* pclass);

px_bool PX_CDA_AddObject(PX_CDA* pCDA, PX_Object* pObject);

PX_Object* PX_CDA_CreateClassObject(PX_CDA* pCDA, px_char classname[], px_float grid_x, px_float grid_y);


px_void PX_CDA_RemoveObject(PX_CDA* pCDA, PX_Object* pObject);

px_void PX_CDA_SetBackwardTexture(PX_CDA* pCDA, px_texture* ptexture);

px_void PX_CDA_SetViewGridSize(PX_CDA* pCDA, px_int size);

px_void PX_CDA_ShowGridLine(PX_CDA* pw, px_bool bline);

px_int PX_CDA_GetObjectCount(PX_CDA* CDA);

px_int PX_CDA_GetAliveCount(PX_CDA* CDA);

px_texture* PX_CDA_TakeSnapShot(PX_CDA* pCDA);


px_bool	PX_CDA_Initialize(px_memorypool* mp, PX_CDA* pCDA, px_int grid_x_count, px_int grid_y_count, px_int view_width, px_int view_height, PX_FontModule* fm, PX_ResourceLibrary* plib);

px_void PX_CDA_Update(PX_CDA* pCDA, px_uint elapsed);

px_void PX_CDA_Render(px_surface* psurface, PX_CDA* pCDA, px_dword elapsed);

px_void PX_CDA_Free(PX_CDA* pCDA);

px_bool PX_CDA_BeginRouteEdit(PX_CDA* pCDA, px_int start_grid_x, px_int start_grid_y, PX_CDA_RouteClass* pRouteClass);

px_bool PX_CDA_RouteMoving(PX_CDA* pCDA,px_int grid_x,px_int grid_y );

px_void PX_CDA_MoveSelectObject(PX_CDA* pCDA, px_float grid_x, px_float grid_y);

px_void PX_CDA_MoveSelectObjectLast(PX_CDA* pCDA, px_float grid_x, px_float grid_y);

px_bool PX_CDA_RoutePin(PX_CDA* pCDA);

px_bool PX_CDA_EndRouteEdit(PX_CDA* pCDA,px_bool save);

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


#endif


