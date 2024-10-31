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
#define PX_OBJECT_EVENT_RESET			    31
//////////////////////////////////////////////////////////////////////////////
//    Type of Controls
/////////////////////////////////////////////////////////////////////////////

enum PX_OBJECT_TYPE
{
  PX_OBJECT_TYPE_NULL		 = 0,
  PX_OBJECT_TYPE_UNKNOW		 = 0,
  PX_OBJECT_TYPE_LABEL			,
  PX_OBJECT_TYPE_PROCESSBAR		,
  PX_OBJECT_TYPE_IMAGE			,
  PX_OBJECT_TYPE_PARTICAL		,
  PX_OBJECT_TYPE_SLIDERBAR		,
  PX_OBJECT_TYPE_LIST			,
  PX_OBJECT_TYPE_LISTITEM		,
  PX_OBJECT_TYPE_BUTTON			,
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
  PX_OBJECT_TYPE_SLIDERTEXT		,
  PX_OBJECT_TYPE_CDA_DESIGNER	,
  PX_OBJECT_TYPE_CDA_OBJECT		,
  PX_OBJECT_TYPE_DRAG,
  PX_OBJECT_TYPE_SCOREPANEL		,
};


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

#define PX_OBJECT_ID_MAXLEN 32
#define PX_OBJECT_MAX_DESC_COUNT 8

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

typedef px_void	 (*Function_ObjectUpdate)(PX_Object *,px_int, px_dword elapsed);
typedef px_void  (*Function_ObjectRender)(px_surface *psurface,PX_Object *, px_int, px_dword elapsed);
typedef px_void  (*Function_ObjectFree)(PX_Object *, px_int);
typedef px_void  (*Function_ObjectLinkChild)(PX_Object *parent,PX_Object *child);

#define PX_OBJECT_EVENT_FUNCTION(name) px_void name(PX_Object *pObject,PX_Object_Event e,px_void * ptr)
#define PX_OBJECT_RENDER_FUNCTION(name) px_void name(px_surface *psurface,PX_Object *pObject,px_int idesc,px_dword elapsed)
#define PX_OBJECT_UPDATE_FUNCTION(name) px_void name(PX_Object *pObject,px_int idesc,px_dword elapsed)
#define PX_OBJECT_FREE_FUNCTION(name) px_void name(PX_Object *pObject,px_int idesc)

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
	px_bool  OnFocus;
	px_bool  OnFocusNode;
	px_bool  OnLostFocusReleaseEvent;
	px_bool  Enabled;
	px_bool  Visible;
	px_bool  ReceiveEvents;
	px_bool  delay_delete;
	px_int   map_index;
	px_dword impact_object_type;
	px_dword impact_target_type;

	union
	{
		px_int  User_int;
		px_void *User_ptr;
	};
	
	//px_int   world_index;
	
	px_int	Type[PX_OBJECT_MAX_DESC_COUNT];
	px_void *pObjectDesc[PX_OBJECT_MAX_DESC_COUNT];
	Function_ObjectUpdate Func_ObjectUpdate[PX_OBJECT_MAX_DESC_COUNT];
	Function_ObjectRender Func_ObjectRender[PX_OBJECT_MAX_DESC_COUNT];
	Function_ObjectFree   Func_ObjectFree[PX_OBJECT_MAX_DESC_COUNT];

	px_memorypool *mp;
	struct _PX_Object *pChilds;
	struct _PX_Object *pParent;
	struct _PX_Object *pPreBrother;
	struct _PX_Object *pNextBrother;

	struct _PX_Object_EventAction *pEventActions; 
	Function_ObjectLinkChild Func_ObjectLinkChild;
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
	px_int  owner;
	px_bool enable;
	px_void (*EventActionFunc)(PX_Object *,PX_Object_Event e,px_void *user_ptr);
	px_void *user_ptr;
	struct _PX_Object_EventAction *pNext;
	struct _PX_Object_EventAction *pPre;
};

typedef struct _PX_Object_EventAction PX_OBJECT_EventAction;

#define    PX_ObjectGetDescIndex(type,pobject,index) ((type *)((pobject)->pObjectDesc[index]))
#define    PX_ObjectGetDesc(type,pobject) PX_ObjectGetDescIndex(type,pobject,idesc)

px_int     PX_ObjectGetFreeDescIndex(PX_Object *pObject);
px_bool    PX_ObjectCheckType(PX_Object* pObject, px_int type);
px_int     PX_ObjectGetTypeIndex(PX_Object* pObject, px_int type);

px_int     PX_ObjectGetDescIndexByType(PX_Object* pObject,px_int type);
px_void*	PX_ObjectGetDescByType(PX_Object* pObject, px_int type);
PX_Object* PX_ObjectCreate(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Length);
px_void	 * PX_ObjectCreateDesc(PX_Object* pObject, px_int idesc, px_int type, Function_ObjectUpdate Func_ObjectUpdate, Function_ObjectRender Func_ObjectRender, Function_ObjectFree Func_ObjectFree, px_void* pDesc, px_int descSize);
PX_Object* PX_ObjectCreateFunction(px_memorypool* mp, PX_Object* Parent, Function_ObjectUpdate Func_ObjectUpdate, Function_ObjectRender Func_ObjectRender, Function_ObjectFree Func_ObjectFree);
PX_Object* PX_ObjectCreateRoot(px_memorypool* mp);
PX_Object* PX_ObjectCreateEx(px_memorypool* mp, PX_Object* Parent, px_float x, px_float y, px_float z, px_float Width, px_float Height, px_float Lenght, px_int type, Function_ObjectUpdate Func_ObjectUpdate, Function_ObjectRender Func_ObjectRender, Function_ObjectFree Func_ObjectFree, px_void* desc, px_int size);
px_void    PX_ObjectDetach(PX_Object *pObject,px_int type);
px_int	   PX_ObjectSetRenderFunction(PX_Object* pObject, Function_ObjectRender Func_ObjectRender, px_int index);
px_int	   PX_ObjectSetUpdateFunction(PX_Object* pObject, Function_ObjectUpdate Func_ObjectUpdate, px_int index);
px_int	   PX_ObjectSetFreeFunction(PX_Object* pObject, Function_ObjectFree Func_ObjectFree, px_int index);

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
px_bool		PX_ObjectIsOnFocus(PX_Object* pObject);
px_void     PX_ObjectClearFocus(PX_Object *pObject);

#define		PX_ObjectReleaseFocus PX_ObjectClearFocus

px_bool		PX_ObjectIsPointInRegion(PX_Object *pObject,px_float x,px_float y);
px_bool		PX_ObjectIsCursorInRegion(PX_Object *pObject,PX_Object_Event e);
px_bool		PX_ObjectIsPointInRegionAlign(PX_Object* pObject, px_float x, px_float y, PX_ALIGN align);
px_bool		PX_ObjectIsCursorInRegionAlign(PX_Object* pObject, PX_Object_Event e, PX_ALIGN align);
px_float	PX_ObjectGetHeight(PX_Object *pObject);
px_float	PX_ObjectGetWidth(PX_Object *pObject);

px_void PX_ObjectAddChild(PX_Object *Parent,PX_Object *child);
px_void PX_ObjectUpdate(PX_Object *pObject,px_uint elapsed );
px_void PX_ObjectUpdatePhysics(PX_Object* pObject, px_uint elapsed);
px_void PX_ObjectRender(px_surface *pSurface,PX_Object *pObject,px_uint elapsed);

px_void PX_ObjectEnableEventAction(PX_Object* pObject, px_int ownerType);
px_void PX_ObjectDisableEventAction(PX_Object *pObject,px_int ownerType);
px_void PX_ObjectEnableNoTheEventAction(PX_Object* pObject, px_int ownerType);
px_void PX_ObjectDisableNoTheEventAction(PX_Object* pObject, px_int ownerType);

px_bool PX_ObjectRegisterEvent(PX_Object *pObject,px_uint Event,px_void (*ProcessFunc)(PX_Object *,PX_Object_Event e,px_void *user_ptr),px_void *ptr);
px_bool PX_ObjectRegisterEventEx(PX_Object* pObject, px_uint Event,px_int owner,px_void(*ProcessFunc)(PX_Object*, PX_Object_Event e, px_void* user_ptr), px_void* ptr);
px_void PX_ObjectRemoveEvent(PX_Object *pObject,px_uint Event);
px_void PX_ObjectPostEvent(PX_Object *pPost,PX_Object_Event Event);
px_void PX_ObjectExecuteEvent(PX_Object *pPost,PX_Object_Event Event);

typedef struct
{

	px_memorypool *mp,calc_mp;
	PX_Quadtree Impact_Test_array[sizeof(px_dword) * 8];
}PX_Object_CollisionTest;

px_bool PX_ObjectCollisionTestCreate(px_memorypool* mp, PX_Object_CollisionTest* ptest, px_int width, px_int height, px_vector* pObjects);
px_int	PX_ObjectSearchRegion(PX_Object_CollisionTest* ptest, px_float x, px_float y, px_float raduis, PX_Object* pObject[], px_int MaxSearchCount, px_dword impact_test_type);
px_void PX_ObjectCollisionTestFree(PX_Object_CollisionTest* ptest);


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

//////////////////////////////////////////////////////////////////////////
//Button
#include "PX_Object_Button.h"

//////////////////////////////////////////////////////////////////////////
//Drag
#include "PX_Object_Drag.h"

//////////////////////////////////////////////////////////////////////////
//ScorePanel
#include "PX_Object_ScorePanel.h"
#endif



