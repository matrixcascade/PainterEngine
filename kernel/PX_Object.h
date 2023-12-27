#ifndef PIXELSES_OBJECT_H
#define PIXELSES_OBJECT_H

#include "../core/PX_Core.h"
#include "PX_Animation.h"
#include "PX_Partical.h"


//////////////////////////////////////////////////////////////////////////
/// Common Events
//////////////////////////////////////////////////////////////////////////
#define PX_OBJECT_EVENT_ANY					0
#define PX_OBJECT_EVENT_CURSORMOVE			1
#define PX_OBJECT_EVENT_CURSORUP			2
#define PX_OBJECT_EVENT_CURSORRDOWN			3
#define PX_OBJECT_EVENT_CURSORDOWN			4
#define PX_OBJECT_EVENT_CURSORRUP			5
#define PX_OBJECT_EVENT_CURSOROVER			6
#define PX_OBJECT_EVENT_CURSOROUT			7
#define PX_OBJECT_EVENT_CURSORWHEEL         8
#define PX_OBJECT_EVENT_CURSORCLICK			9
#define PX_OBJECT_EVENT_CURSORDRAG			10
#define PX_OBJECT_EVENT_STRING				11
#define PX_OBJECT_EVENT_EXECUTE				12
#define PX_OBJECT_EVENT_VALUECHANGED		13
#define PX_OBJECT_EVENT_DRAGFILE			14
#define PX_OBJECT_EVENT_KEYDOWN				15
#define PX_OBJECT_EVENT_KEYUP				16
#define PX_OBJECT_EVENT_IMPACT				17
#define PX_OBJECT_EVENT_SCALE               18
#define PX_OBJECT_EVENT_WINDOWRESIZE        19
#define PX_OBJECT_EVENT_ONFOCUS				20
#define PX_OBJECT_EVENT_LOSTFOCUS           21
#define PX_OBJECT_EVENT_CANCEL				22
#define PX_OBJECT_EVENT_CLOSE				23
#define PX_OBJECT_EVENT_CURSORMUP			24
#define PX_OBJECT_EVENT_CURSORMDOWN			25
#define PX_OBJECT_EVENT_REQUESTDATA			26
#define PX_OBJECT_EVENT_OPEN				27
#define PX_OBJECT_EVENT_SAVE				28
#define PX_OBJECT_EVENT_TIMEOUT				29
#define PX_OBJECT_EVENT_DAMAGE				30
//////////////////////////////////////////////////////////////////////////////
//    Type of Controls
/////////////////////////////////////////////////////////////////////////////

enum PX_OBJECT_TYPE
{
  PX_OBJECT_TYPE_NULL			,
  PX_OBJECT_TYPE_LABEL			,
  PX_OBJECT_TYPE_PROCESSBAR		,
  PX_OBJECT_TYPE_IMAGE			,
  PX_OBJECT_TYPE_PARTICAL		,
  PX_OBJECT_TYPE_SLIDERBAR		,
  PX_OBJECT_TYPE_LIST			,
  PX_OBJECT_TYPE_LISTITEM		,
  PX_OBJECT_TYPE_PUSHBUTTON		,
  PX_OBJECT_TYPE_EDIT           ,
  PX_OBJECT_TYPE_SCROLLAREA     ,
  PX_OBJECT_TYPE_AUTOTEXT		,
  PX_OBJECT_TYPE_ANIMATION		,
  PX_OBJECT_TYPE_CURSORBUTTON   ,
  PX_OBJECT_TYPE_VKEYBOARD		,
  PX_OBJECT_TYPE_VNKEYBOARD		,
  PX_OBJECT_TYPE_OSCILLOSCOPE	,
  PX_OBJECT_TYPE_FILTEREDITOR   ,
  PX_OBJECT_TYPE_CHECKBOX		,
  PX_OBJECT_TYPE_ROTATION		,
  PX_OBJECT_TYPE_MENU			,
  PX_OBJECT_TYPE_SELECTBAR		,
  PX_OBJECT_TYPE_RADIOBOX		,
  PX_OBJECT_TYPE_EXPLORER		,
  PX_OBJECT_TYPE_WIDGET			,
  PX_OBJECT_TYPE_SCALEBOX		,
  PX_OBJECT_TYPE_MESSAGEBOX		,
  PX_OBJECT_TYPE_PROTRACTOR		,
  PX_OBJECT_TYPE_TRANSFORMADAPTER,
  PX_OBJECT_TYPE_PANC			,
  PX_OBJECT_TYPE_DESIGNERBOX	,
  PX_OBJECT_TYPE_JOYSTICK		,
  PX_OBJECT_TYPE_MEMORYVIEW		,
  PX_OBJECT_TYPE_VARIOUS		,
  PX_OBJECT_TYPE_IPBOX			,
  PX_OBJECT_TYPE_VARBOX			,
  PX_OBJECT_TYPE_MODBUSVARBOX	,
  PX_OBJECT_TYPE_COMBOX			,
  PX_OBJECT_TYPE_RANKPANEL		,
  PX_OBJECT_TYPE_RINGPROGRESS	,
  PX_OBJECT_TYPE_RINGPROCESSBAR	,
  PX_OBJECT_TYPE_PIANO			,
  PX_OBJECT_TYPE_PIANOTUNE		,
  PX_OBJECT_TYPE_COUNTERDOWN	,
  PX_OBJECT_TYPE_PRINTER		,
  PX_OBJECT_TYPE_EXECUTER		,
  PX_OBJECT_TYPE_ASMDEBUGGER	,
  PX_OBJECT_TYPE_EXPLOSIONX  	,
  PX_OBJECT_TYPE_COLORPANEL		,
  PX_OBJECT_TYPE_PAINTERBOX		,
  PX_OBJECT_TYPE_LAYERBOX		,
  PX_OBJECT_TYPE_CANVAS			,
  PX_OBJECT_TYPE_CURSORSLIDER   ,
  PX_OBJECT_TYPE_TYPER			,
  PX_OBJECT_TYPE_TIMER			,
  PX_OBJECT_TYPE_PANEL			,
  PX_OBJECT_TYPE_3DMODEL		,
  PX_OBJECT_TYPE_SOUNDVIEW		,
  PX_OBJECT_TYPE_LIVE2D			,
  PX_OBJECT_TYPE_FIREWORK		,
  PX_OBJECT_TYPE_MATRIXEFFECT   ,
};


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

#define PX_OBJECT_ID_MAXLEN 32

#define  PX_OBJECT_IMAGE_LISTITEM_STYLE_NONE		0
#define  PX_OBJECT_IMAGE_LISTITEM_STYLE_BORDER		1

#define  PX_OBJECT_IMAGE_LISTBOX_ITEMHEIGHT			18
#define  PX_OBJECT_IMAGE_LISTBOX_SLIDERWITDH		8

#define  PX_OBJECT_IMAGE_LISTBOX_STYLE_ITEMBORDER	1
#define  PX_OBJECT_IMAGE_LISTBOX_STYLE_SELECT		2

#define PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR PX_COLOR_BACKGROUNDCOLOR
#define PX_OBJECT_UI_DEFAULT_CURSORCOLOR PX_COLOR_CURSORCOLOR
#define PX_OBJECT_UI_DEFAULT_FONTCOLOR PX_COLOR_FONTCOLOR
#define PX_OBJECT_UI_DEFAULT_BORDERCOLOR PX_COLOR_BORDERCOLOR
#define PX_OBJECT_UI_DEFAULT_PUSHCOLOR PX_COLOR_PUSHCOLOR


#define PX_OBJECT_UI_DISABLE_FONTCOLOR PX_COLOR(255,128,128,128)
#define  PX_OBJECT_FILTEREDITOR_DEFAULE_HORIZONTALPIXELDIVIDING 48
#define  PX_OBJECT_FILTEREDITOR_DEFAULE_VERTICALPIXELDIVIDING   20
#define	 PX_OBJECT_FILTEREDITOR_DEFAULT_FRAMELINE_WIDTH			   2


#define PX_OBJECT_INHERIT_CODE(__pObject, objx, objy, objWidth,objHeight) do{px_float ix, iy;PX_ObjectGetInheritXY(__pObject, &ix, &iy);objx = (__pObject->x + ix);objy = (__pObject->y + iy);objWidth = __pObject->Width;objHeight = __pObject->Height;}while(0);


///////////////////////////////////////////////////////////////////////////
////  PainterEngine pObject
//////////////////////////////////////////////////////////////////////////
struct _PX_Object;
typedef struct _PX_Object PX_Object;

typedef px_void  (*Function_ObjectUpdate)(PX_Object *,px_uint elapsed);
typedef px_void  (*Function_ObjectBeginRender)(px_surface *,PX_Object *,px_dword);
typedef px_void  (*Function_ObjectRender)(px_surface *psurface,PX_Object *,px_uint elapsed);
typedef px_void  (*Function_ObjectEndRender)(px_surface *,PX_Object *,px_dword);
typedef px_void  (*Function_ObjectFree)(PX_Object *);
typedef px_void  (*Function_ObjectLinkChild)(PX_Object *parent,PX_Object *child);

#define PX_OBJECT_EVENT_FUNCTION(name) px_void name(PX_Object *pObject,PX_Object_Event e,px_void * ptr)
#define PX_OBJECT_RENDER_FUNCTION(name) px_void name(px_surface *psurface,PX_Object *pObject,px_uint elapsed)
#define PX_OBJECT_UPDATE_FUNCTION(name) px_void name(PX_Object *pObject,px_uint elapsed)
#define PX_OBJECT_FREE_FUNCTION(name) px_void name(PX_Object *pObject)

#define PX_OBJECT_EVENTFUNCTION(name) PX_OBJECT_EVENT_FUNCTION(name)
#define PX_OBJECT_RENDERFUNCTION(name) PX_OBJECT_RENDER_FUNCTION(name)
#define PX_OBJECT_UPDATEFUNCTION(name) PX_OBJECT_UPDATE_FUNCTION(name)
#define PX_OBJECT_FREEFUNCTION(name) PX_OBJECT_FREE_FUNCTION(name)

struct _PX_Object
{
	px_char id[PX_OBJECT_ID_MAXLEN];
	px_float x, y, z;
	px_float vx, vy, vz;
	px_float fx, fy, fz;
	px_float ak;

	px_float Width;
	px_float Height;
	px_float Length;
	px_float diameter;//if the member is not zero,The pObject is round shape
	px_bool OnFocus;
	px_bool OnFocusNode;
	px_bool OnLostFocusReleaseEvent;
	px_bool Enabled;
	px_bool Visible;
	px_bool ReceiveEvents;
	px_int	Type;
	px_int  designerType;
	union
	{
	px_int  User_int;
	px_void *User_ptr;
	};
	
	px_int   world_index;
	px_bool  delay_delete;
	px_dword impact_object_type;
	px_dword impact_target_type;
	
	px_void *pObjectDesc;
	px_memorypool *mp;
	struct _PX_Object *pChilds;
	struct _PX_Object *pParent;
	struct _PX_Object *pPreBrother;
	struct _PX_Object *pNextBrother;

	struct _PX_Object_EventAction *pEventActions; 
	Function_ObjectUpdate Func_ObjectUpdate;
	Function_ObjectRender Func_ObjectRender;
	Function_ObjectFree   Func_ObjectFree;
	Function_ObjectLinkChild Func_ObjectLinkChild;
	Function_ObjectBeginRender Func_ObjectBeginRender;
	Function_ObjectEndRender Func_ObjectEndRender;
};


//////////////////////////////////////////////////////////////////////////
///   Controllers
//////////////////////////////////////////////////////////////////////////

typedef struct
{
	px_shape *shape;
	px_point cursorPoint;
	px_color blendColor;
}PX_Object_RoundCursor;



typedef struct _PX_Object_Event
{
	px_uint Event;
	union
	{
		px_char Param_char[16];
		px_uint Param_uint[4];
		px_int Param_int[4];
		px_float Param_float[4];
		px_void *Param_ptr[4];
		px_bool Param_bool[4];
	};
}PX_Object_Event;

//////////////////////////////////////////////////////////////////////////
//EVENT params
PX_Object_Event PX_OBJECT_BUILD_EVENT(px_uint Event);
PX_Object_Event PX_OBJECT_BUILD_EVENT_STRING(px_uint Event,const px_char *content);
PX_Object_Event PX_OBJECT_BUILD_EVENT_INT(px_uint Event, px_int i);
PX_Object_Event PX_Object_Event_CursorOffset(PX_Object_Event e, px_point offset);

px_float PX_Object_Event_GetCursorX(PX_Object_Event e);
px_float PX_Object_Event_GetCursorY(PX_Object_Event e);
px_float PX_Object_Event_GetCursorZ(PX_Object_Event e);
px_float PX_Object_Event_GetCursorPressure(PX_Object_Event e);
px_int PX_Object_Event_GetCursorIndex(PX_Object_Event e);
px_float PX_Object_Event_GetDamage(PX_Object_Event e);

px_float PX_Object_Event_GetWidth(PX_Object_Event e);
px_float PX_Object_Event_GetHeight(PX_Object_Event e);
px_int PX_Object_Event_GetIndex(PX_Object_Event e);

px_void PX_Object_Event_SetImpactTargetObject(PX_Object_Event* e,PX_Object* pObject);
PX_Object* PX_Object_Event_GetImpactTargetObject(PX_Object_Event e);

px_int PX_Object_Event_GetInt(PX_Object_Event e);
px_void PX_Object_Event_SetWidth(PX_Object_Event* e, px_float w);
px_void PX_Object_Event_SetHeight(PX_Object_Event *e,px_float h);
px_void PX_Object_Event_SetCursorX(PX_Object_Event *e,px_float x);
px_void PX_Object_Event_SetCursorY(PX_Object_Event *e,px_float y);
px_void PX_Object_Event_SetCursorZ(PX_Object_Event *e,px_float z);
px_void PX_Object_Event_SetDamage(PX_Object_Event* e, px_float damage);
px_void PX_Object_Event_SetCursorIndex(PX_Object_Event *e,px_int index);

px_float PX_Object_Event_GetScaleCursorX(PX_Object_Event e);
px_float PX_Object_Event_GetScaleCursorY(PX_Object_Event e);
px_float PX_Object_Event_GetScaleCursorZ(PX_Object_Event e);

px_void PX_Object_Event_SetScaleCursorX(PX_Object_Event *e,px_float x);
px_void PX_Object_Event_SetScaleCursorY(PX_Object_Event *e,px_float y);
px_void PX_Object_Event_SetScaleCursorZ(PX_Object_Event *e,px_float z);

px_uint PX_Object_Event_GetKeyDown(PX_Object_Event e);
px_void PX_Object_Event_SetKeyDown(PX_Object_Event *e,px_uint key);
px_void PX_Object_Event_SetKeyUp(PX_Object_Event *e,px_uint key);
px_char* PX_Object_Event_GetStringPtr(PX_Object_Event e);
px_void* PX_Object_Event_GetDataPtr(PX_Object_Event e);
px_void PX_Object_Event_SetStringPtr(PX_Object_Event *e,px_void *ptr);
px_void PX_Object_Event_SetDataPtr(PX_Object_Event *e,px_void *ptr);
px_void PX_Object_Event_SetIndex(PX_Object_Event *e,px_int index);
struct _PX_Object_EventAction
{
	px_uint EventAction;
	px_void (*EventActionFunc)(PX_Object *,PX_Object_Event e,px_void *user_ptr);
	px_void *user_ptr;
	struct _PX_Object_EventAction *pNext;
	struct _PX_Object_EventAction *pPre;
};

typedef struct _PX_Object_EventAction PX_OBJECT_EventAction;


PX_Object *PX_ObjectCreate(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Length);
PX_Object *PX_ObjectCreateEx(px_memorypool *mp,PX_Object *Parent,\
	px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Length,\
	px_int type,\
	Function_ObjectUpdate Func_ObjectUpdate,\
	Function_ObjectRender Func_ObjectRender,\
	Function_ObjectFree   Func_ObjectFree,\
	px_void *desc,\
	px_int size
	);
PX_Object* PX_ObjectCreateFunction(px_memorypool* mp, PX_Object* Parent, Function_ObjectUpdate Func_ObjectUpdate, Function_ObjectRender Func_ObjectRender, Function_ObjectFree Func_ObjectFree);
PX_Object* PX_ObjectCreateRoot(px_memorypool* mp);
#define    PX_ObjectGetDesc(type,pobject) ((type *)((pobject)->pObjectDesc))
px_void    PX_ObjectGetInheritXY(PX_Object *pObject,px_float *x,px_float *y);
px_void	   PX_ObjectInitialize(px_memorypool *mp,PX_Object *pObject,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Length);
px_void    PX_ObjectSetId(PX_Object *pObject,const px_char id[]);
px_void    PX_ObjectSetUserCode(PX_Object *pObject,px_int user_int);
px_void    PX_ObjectSetUserPointer(PX_Object *pObject,px_void *user_ptr);
px_void	   PX_ObjectSetParent(PX_Object* pObject, PX_Object* Parent);
px_void    PX_ObjectDelete(PX_Object *pObject);
px_void    PX_ObjectDelayDelete(PX_Object* pObject);
px_void	   PX_ObjectDeleteChilds( PX_Object *pObject );
px_void	   PX_ObjectSetPosition(PX_Object *pObject,px_float x,px_float y,px_float z);
px_void	   PX_ObjectSetForce(PX_Object* pObject, px_float x, px_float y, px_float z);
px_void	   PX_ObjectSetVelocity(PX_Object* pObject, px_float x, px_float y, px_float z);
px_void	   PX_ObjectSetResistance(PX_Object* pObject, px_float ak);
px_void    PX_ObjectSetSize(PX_Object *pObject,px_float Width,px_float Height,px_float length);
px_void	   PX_ObjectSetVisible(PX_Object *pObject,px_bool visible);
px_void    PX_ObjectSetEnabled(PX_Object *pObject,px_bool enabled);
px_void    PX_ObjectEnable(PX_Object *pObject);
px_void    PX_ObjectDisable(PX_Object *pObject);
PX_Object  *PX_ObjectGetChild(PX_Object *pObject,px_int Index);
PX_Object  *PX_ObjectGetObject(PX_Object *pObject,const px_char payload[]);
px_void     PX_ObjectSetFocus(PX_Object *pObject);
px_void     PX_ObjectClearFocus(PX_Object *pObject);
#define		PX_ObjectReleaseFocus PX_ObjectClearFocus

px_bool		PX_ObjectIsPointInRegion(PX_Object *pObject,px_float x,px_float y);
px_bool		PX_ObjectIsCursorInRegion(PX_Object *pObject,PX_Object_Event e);
px_float	PX_ObjectGetHeight(PX_Object *pObject);
px_float	PX_ObjectGetWidth(PX_Object *pObject);

px_void PX_ObjectAddChild(PX_Object *Parent,PX_Object *child);
px_void PX_ObjectUpdate(PX_Object *pObject,px_uint elapsed );
px_void PX_ObjectUpdatePhysics(PX_Object* pObject, px_uint elapsed);
px_void PX_ObjectRender(px_surface *pSurface,PX_Object *pObject,px_uint elapsed);

px_int PX_ObjectRegisterEvent(PX_Object *pObject,px_uint Event,px_void (*ProcessFunc)(PX_Object *,PX_Object_Event e,px_void *user_ptr),px_void *ptr);
px_void PX_ObjectPostEvent(PX_Object *pPost,PX_Object_Event Event);
px_void PX_ObjectExecuteEvent(PX_Object *pPost,PX_Object_Event Event);


//////////////////////////////////////////////////////////////////////////
//designer
/////////////////////////////////////////////////////////////////////////
#define PX_DESIGNER_NAME_LENGTH 48
#define PX_DESIGNER_MAX_PROPERTYS 32
#define PX_DESIGNER_CONTROLLER_ITEM_HEIGHT 22
typedef enum
{
	PX_DESIGNER_OBJECT_TYPE_UI,
	PX_DESIGNER_OBJECT_TYPE_GAME,
	PX_DESIGNER_OBJECT_TYPE_FUNCTION,
}PX_DESIGNER_OBJECT_TYPE;


typedef PX_Object* (*px_designer_createfunc)(px_memorypool* mp, PX_Object* pparent, px_float x, px_float y, px_float width, px_float height, px_void* userptr);

typedef px_float(*px_designer_getproperty_float)(PX_Object* pObject);
typedef px_int(*px_designer_getproperty_int)(PX_Object* pObject);
typedef px_bool(*px_designer_getproperty_string)(PX_Object* pObject, px_string* str);
typedef px_bool(*px_designer_getproperty_bool)(PX_Object* pObject);

typedef px_void(*px_designer_setproperty_float)(PX_Object* pObject, px_float v);
typedef px_void(*px_designer_setproperty_int)(PX_Object* pObject, px_int v);
typedef px_void(*px_designer_setproperty_string)(PX_Object* pObject, const px_char v[]);
typedef px_void(*px_designer_setproperty_bool)(PX_Object* pObject, px_bool v);

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

//common
px_bool PX_Designer_GetID(PX_Object* pObject, px_string* str);
px_float PX_Designer_GetX(PX_Object* pObject);
px_float PX_Designer_GetY(PX_Object* pObject);
px_float PX_Designer_GetWidth(PX_Object* pObject);
px_float PX_Designer_GetHeight(PX_Object* pObject);
px_bool PX_Designer_GetEnable(PX_Object* pObject);

px_void PX_Designer_SetID(PX_Object* pObject, const px_char id[]);
px_void PX_Designer_SetX(PX_Object* pObject, px_float v);
px_void PX_Designer_SetY(PX_Object* pObject, px_float v);
px_void PX_Designer_SetHeight(PX_Object* pObject, px_float v);
px_void PX_Designer_SetWidth(PX_Object* pObject, px_float v);
px_void PX_Designer_SetEnable(PX_Object* pObject, px_bool v);


//////////////////////////////////////////////////////////////////////////
//Label
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_Label.h"

//////////////////////////////////////////////////////////////////////////
//ProcessBar
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_Processbar.h"

//////////////////////////////////////////////////////////////////////////
//Image
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_Image.h"

//////////////////////////////////////////////////////////////////////////
//SliderBar
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_Sliderbar.h"

//////////////////////////////////////////////////////////////////////////
//PushButton
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_PushButton.h"

//////////////////////////////////////////////////////////////////////////
//Edit
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_Edit.h"

/////////////////////////////////////////////////////////////////////////////////////////
//ScrollArea
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_ScrollArea.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//AutoText
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_AutoText.h"

//////////////////////////////////////////////////////////////////////////
//Animation
#include "PX_Object_Animation.h"

//////////////////////////////////////////////////////////////////////////
//CursorButton
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_CursorButton.h"

//////////////////////////////////////////////////////////////////////////
//list
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_List.h"
//////////////////////////////////////////////////////////////////////////
//virtual keyboard
#include "PX_Object_VirtualKeyboard.h"
//////////////////////////////////////////////////////////////////////////
//coordinate
#include "PX_Object_Oscilloscope.h"

//////////////////////////////////////////////////////////////////////////
//checkbox
#include "PX_Object_CheckBox.h"
//////////////////////////////////////////////////////////////////////////
//menu
#include "PX_Object_Menu.h"

//////////////////////////////////////////////////////////////////////////
//selectbar
#include "PX_Object_Selectbar.h"

//////////////////////////////////////////////////////////////////////////
//radiobox
#include "PX_Object_RadioBox.h"

//////////////////////////////////////////////////////////////////////////
//explorer
#include "PX_Object_Explorer.h"
//////////////////////////////////////////////////////////////////////////
//widget
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_Widget.h"


//////////////////////////////////////////////////////////////////////////
//scalebox
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_ScaleBox.h"



//////////////////////////////////////////////////////////////////////////
//messagebox
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_MessageBox.h"


//////////////////////////////////////////////////////////////////////////
//protractor
//////////////////////////////////////////////////////////////////////////
#include "PX_Object_Protractor.h"

//////////////////////////////////////////////////////////////////////////
//transform adapter
#include "PX_Object_TransformAdapter.h"

//////////////////////////////////////////////////////////////////////////
//panc
#include "PX_Object_Panc.h"


//////////////////////////////////////////////////////////////////////////
//designerbox
#include "PX_Object_DesignerBox.h"

//////////////////////////////////////////////////////////////////////////
//joystick
#include "PX_Object_Joystick.h"

//////////////////////////////////////////////////////////////////////////
//ringprocessbar
#include "PX_Object_RingProcessbar.h"

//////////////////////////////////////////////////////////////////////////
//memoryview
#include "PX_Object_MemoryView.h"

//////////////////////////////////////////////////////////////////////////
//various
#include "PX_Object_Various.h"


//////////////////////////////////////////////////////////////////////////
//ipbox
#include "PX_Object_IPBox.h"

//////////////////////////////////////////////////////////////////////////
//varbox
#include "PX_Object_VarBox.h"
#include "PX_Object_ModbusVarBox.h"

//////////////////////////////////////////////////////////////////////////
//combox
#include "PX_Object_COMBox.h"

//////////////////////////////////////////////////////////////////////////
//RankPanel
#include "PX_Object_RankPanel.h"

//////////////////////////////////////////////////////////////////////////
//ring progress
#include "PX_Object_RingProgress.h"


//////////////////////////////////////////////////////////////////////////
//piano
#include "PX_Object_Piano.h"

//////////////////////////////////////////////////////////////////////////
//counter down
#include "PX_Object_CounterDown.h"

//////////////////////////////////////////////////////////////////////////
//printer
#include "PX_Object_Printer.h"

//////////////////////////////////////////////////////////////////////////
//execute
#include "PX_Object_Executer.h"

//////////////////////////////////////////////////////////////////////////
//execute
#include "PX_Object_AsmDebugger.h"

//////////////////////////////////////////////////////////////////////////
//explosion
#include "PX_Object_Explosion.h"

//////////////////////////////////////////////////////////////////////////
//color panel
#include "PX_Object_ColorPanel.h"

//////////////////////////////////////////////////////////////////////////
//PainterBox
#include "PX_Object_PainterBox.h"

//////////////////////////////////////////////////////////////////////////
//LayerBox
#include "PX_Object_LayerBox.h"

//////////////////////////////////////////////////////////////////////////
//Canvas
#include "PX_Object_Canvas.h"

//////////////////////////////////////////////////////////////////////////
//SliderCursor
#include "PX_Object_CursorSlider.h"

//////////////////////////////////////////////////////////////////////////
//Typer
#include "PX_Object_Typer.h"

//////////////////////////////////////////////////////////////////////////
//Timer
#include "PX_Object_Timer.h"

//////////////////////////////////////////////////////////////////////////
//Panel
#include "PX_Object_Panel.h"

//////////////////////////////////////////////////////////////////////////
//3D
#include "PX_Object_3D.h"

//////////////////////////////////////////////////////////////////////////
//SoundView
#include "PX_Object_SoundView.h"

//////////////////////////////////////////////////////////////////////////
//Partical
#include "PX_Object_Partical.h"

//////////////////////////////////////////////////////////////////////////
//Live2D
#include "PX_Object_Live2D.h"

/////////////////////////////////////////////////////////////////////////
//Firework
#include "PX_Object_Firework.h"

//////////////////////////////////////////////////////////////////////////
//MatrixEffect
#include "PX_Object_MatrixEffect.h"

#endif



