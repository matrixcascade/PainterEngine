#ifndef PIXELSES_OBJECT_H
#define PIXELSES_OBJECT_H

#include "../Core/PX_Core.h"
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
#define PX_OBJECT_EVENT_STRING				8
#define PX_OBJECT_EVENT_CURSORCLICK			9
#define PX_OBJECT_EVENT_CURSORDRAG			10
#define PX_OBJECT_EVENT_VALUECHAGE			11
#define PX_OBJECT_EVENT_DRAGFILE			12
#define PX_OBJECT_EVENT_KEYDOWN				13
#define PX_OBJECT_EVENT_IMPACT				14

//////////////////////////////////////////////////////////////////////////////
//    Type of Controls
/////////////////////////////////////////////////////////////////////////////

enum PX_OBJECT_TYPE
{
  PX_OBJECT_TYPE_NULL			,
  PX_OBJECT_TYPE_LABEL			,
  PX_OBJECT_TYPE_PROCESSBAR		,
  PX_OBJECT_TYPE_RADIUSICO		,
  PX_OBJECT_TYPE_IMAGE			,
  PX_OBJECT_TYPE_SHAPE			,
  PX_OBJECT_TYPE_SLIDERBAR		,
  PX_OBJECT_TYPE_LISTITEM		,
  PX_OBJECT_TYPE_PUSHBUTTON		,
  PX_OBJECT_TYPE_ROTATION		,
  PX_OBJECT_TYPE_EDIT           ,
  PX_OBJECT_TYPE_STATICIMAGE    ,
  PX_OBJECT_TYPE_SCROLLAREA     ,
  PX_OBJECT_TYPE_AUTOTEXT		,
  PX_OBJECT_TYPE_ANIMATION		,
  PX_OBJECT_TYPE_PARTICAL		,
  PX_OBJECT_TYPE_ROUNDCURSOR    ,
  PX_OBJECT_TYPE_CURSORBUTTON   ,
};


#define  PX_OBJECT_ALIGN_HCENTER		1
#define  PX_OBJECT_ALIGN_VCENTER		2
#define  PX_OBJECT_ALIGN_LEFT			4
#define  PX_OBJECT_ALIGN_RIGHT			8
#define  PX_OBJECT_ALIGN_TOP			16
#define  PX_OBJECT_ALIGN_BOTTOM			32

#define  PX_OBJECT_ALIGN_CENTER     (PX_OBJECT_ALIGN_HCENTER|PX_OBJECT_ALIGN_VCENTER)

//////////////////////////////////////////////////////////////////////////
//              Label Control
//////////////////////////////////////////////////////////////////////////

enum PX_OBJECT_SLIDERBAR_TYPE
{
 PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL		,
 PX_OBJECT_SLIDERBAR_TYPE_VERTICAL			,
};

enum PX_OBJECT_SLIDERBAR_STYLE
{
	PX_OBJECT_SLIDERBAR_STYLE_BOX			,
	PX_OBJECT_SLIDERBAR_STYLE_LINER			,
};

enum PX_OBJECT_SLIDERBAR_STATUS
{
	PX_OBJECT_SLIDERBAR_STATUS_ONDRAG		,
	PX_OBJECT_SLIDERBAR_STATUS_NORMAL		,
};

#define  PX_OBJECT_IMAGE_LISTITEM_STYLE_NONE		0
#define  PX_OBJECT_IMAGE_LISTITEM_STYLE_BORDER		1

#define  PX_OBJECT_IMAGE_LISTBOX_ITEMHEIGHT			18
#define  PX_OBJECT_IMAGE_LISTBOX_SLIDERWITDH		8

#define  PX_OBJECT_IMAGE_LISTBOX_STYLE_ITEMBORDER	1
#define  PX_OBJECT_IMAGE_LISTBOX_STYLE_SELECT		2




///////////////////////////////////////////////////////////////////////////
////  PixelsES Object
//////////////////////////////////////////////////////////////////////////


struct _PX_Object
{
	px_float x;
	px_float y;
	px_float z;
	px_float Width;
	px_float Height;
	px_float Length;
	px_bool Enabled;
	px_bool Visible;
	px_bool ReceiveEvents;
	union
	{
	px_int  User_int;
	px_void *User_ptr;
	};
	px_dword impact_type;
	px_dword impact_test_type;
	px_void *pObject;
	px_memorypool *mp;
	struct _PX_Object *pChilds;
	struct _PX_Object *pParent;
	struct _PX_Object *pPreBrother;
	struct _PX_Object *pNextBrother;

	struct _PX_Object_EventAction *pEventActions; 
	px_void  (*Func_ObjectUpdate)(struct _PX_Object *,px_uint elpased);
	px_void  (*Func_ObjectRender)(px_surface *psurface,struct _PX_Object *,px_uint elpased);
	px_void  (*Func_ObjectFree)(struct _PX_Object *);
	px_int	Type;
};
typedef struct _PX_Object PX_Object;

//////////////////////////////////////////////////////////////////////////
///   Controllers
//////////////////////////////////////////////////////////////////////////
typedef struct  
{
	px_uchar Align;
	px_bool  Border;
	px_color TextColor;
	px_color BackgroundColor;
	px_char *Text;
}PX_Object_Label;


typedef struct 
{
	px_int Value;
	px_int MAX;
	px_color Color;
	px_color BackgroundColor;
}PX_Object_ProcessBar;

typedef struct 
{
	px_int Radius;
	px_color Color;
}PX_Object_RadiusICO;


typedef struct 
{
	px_uchar Align;
	px_texture *pTexture;
	px_texture *pmask;
}PX_Object_Image;

typedef struct 
{
	px_uchar Align;
	px_color blendcolor;
	px_shape *pShape;
}PX_Object_Shape;

typedef struct 
{
	enum PX_OBJECT_SLIDERBAR_TYPE Type;
	enum PX_OBJECT_SLIDERBAR_STYLE style;
	enum PX_OBJECT_SLIDERBAR_STATUS status;
	px_int btnDownX,btnDownY;
	px_int DargButtonX,DargButtonY;
	px_int Max;
	px_int lastValue;
	px_int Value;
	px_int SliderButtonLength;
	px_color color;
	px_color BackgroundColor;
}PX_Object_SliderBar;


typedef struct 
{
	px_texture *Texture;
	px_uchar Selected;
}PX_Object_SelectButton;

typedef struct 
{
	px_uchar Align;
	px_texture Texture;
}PX_Object_StaticImage;


#define PX_OBJECT_PUSHBUTTON_ROUNDRADIUS 8.0f
typedef enum
{
	PX_OBJECT_BUTTON_STATE_ONCURSOR,
	PX_OBJECT_BUTTON_STATE_ONPUSH,
	PX_OBJECT_BUTTON_STATE_NORMAL,
}PX_Object_PUSHBUTTON_STATE;


typedef enum
{
	PX_OBJECT_PUSHBUTTON_STYLE_RECT,
	PX_OBJECT_PUSHBUTTON_STYLE_ROUNDRECT,
}PX_OBJECT_PUSHBUTTON_STYLE;

typedef struct 
{
	px_uchar Align;
	px_uchar Border;
	px_color TextColor;
	px_color BorderColor;
	px_color BackgroundColor;
	px_color CursorColor;
	px_color PushColor;
	px_char *Text;
	px_texture *Texture;
	px_float roundradius;
	PX_OBJECT_PUSHBUTTON_STYLE style;
	PX_Object_PUSHBUTTON_STATE state;
}PX_Object_PushButton;

typedef struct 
{
	PX_Object *pushbutton;
	px_float c_distance;
	px_float c_distance_far;
	px_float c_distance_near;
	px_color c_color;
	px_int c_width;
	px_bool enter;
}PX_Object_CursorButton;

typedef enum
{
	PX_OBJECT_EDIT_STATE_ONCURSOR,
	PX_OBJECT_EDIT_STATE_NORMAL,
}PX_OBJECT_EDIT_STATE;

typedef struct 
{
	px_int angle_per_second;
	px_float angle;
	px_texture *pTexture,RotationTexture;
}PX_Object_Rotation;

typedef struct 
{
	px_string text;
	px_bool onFocus;
	px_uchar Border;
	px_color TextColor;
	px_color BorderColor;
	px_color CursorColor;
	px_color BackgroundColor;
	px_uint elpased;
	px_bool Password;
	px_bool AutoNewline;
	px_int XOffset,YOffset;
	px_int VerticalOffset,HorizontalOffset;
	px_int xSpacing,ySpacing;
	px_int cursor_index;
	px_int max_length;
	px_surface EditSurface;
	PX_OBJECT_EDIT_STATE state;
}PX_Object_Edit;



typedef struct 
{
	px_int oftx,ofty;
	px_bool bBorder;
	px_surface surface;
	px_color BackgroundColor;
	px_color borderColor;
	PX_Object *Object;
}PX_Object_ScrollArea;

typedef struct 
{
	px_uchar Align;
	px_animation animation;
}PX_Object_Animation;

typedef struct 
{
	px_color TextColor;
	px_string text;
}PX_Object_AutoText;


typedef struct 
{
	PX_Partical_Launcher launcher;
}PX_Object_Partical;



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
		px_void *user;
	};
}PX_Object_Event;



struct _PX_Object_EventAction
{
	px_uint EventAction;
	px_void (*EventActionFunc)(PX_Object *,PX_Object_Event e,px_void *user_ptr);
	px_void *user_ptr;
	struct _PX_Object_EventAction *pNext;
	struct _PX_Object_EventAction *pPre;
};

typedef struct _PX_Object_EventAction PX_OBJECT_EventAction;


//PX_Object *PX_ObjectRootCreate(px_memorypool *mp);
//px_void	   PX_ObjectRootInit(PX_Object *Object,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Lenght);
PX_Object *PX_ObjectCreate(px_memorypool *mp,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Lenght);
px_void	   PX_ObjectInit(px_memorypool *mp,PX_Object *Object,PX_Object *Parent,px_float x,px_float y,px_float z,px_float Width,px_float Height,px_float Lenght);
px_void    PX_ObjectSetUserCode(PX_Object *pObject,px_int user_int);
px_void    PX_ObjectSetUserPointer(PX_Object *pObject,px_void *user_ptr);
px_void    PX_ObjectDelete(PX_Object *pObject);
px_void	   PX_ObjectDeleteChilds( PX_Object *pObject );
px_void	   PX_ObjectSetPosition(PX_Object *Object,px_float x,px_float y,px_float z);
px_void    PX_ObjectSetSize(PX_Object *Object,px_float Width,px_float Height,px_float length);
px_void	   PX_ObjectSetVisible(PX_Object *Object,px_bool visible);
PX_Object  *PX_ObjectGetChild(PX_Object *Object,px_int Index);
px_bool		PX_ObjectIsPointInRegion(PX_Object *Object,px_float x,px_float y);
px_float	PX_ObjectGetHeight(PX_Object *Object);
px_float	PX_ObjectGetWidth(PX_Object *Object);

px_void PX_ObjectUpdate(PX_Object *Object,px_uint elpased );
px_void PX_ObjectRender(px_surface *pSurface,PX_Object *Object,px_uint elpased);

px_int PX_ObjectRegisterEvent(PX_Object *Object,px_uint Event,px_void (*ProcessFunc)(PX_Object *,PX_Object_Event e,px_void *user_ptr),px_void *ptr);
px_void PX_ObjectPostEvent(PX_Object *pPost,PX_Object_Event Event);
px_void PX_ObjectExecuteEvent(PX_Object *pPost,PX_Object_Event Event);




PX_Object *	PX_Object_LabelCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_char *Text,px_color Color);
PX_Object_Label  *	PX_Object_GetLabel(PX_Object *Object);
px_char	  * PX_Object_LabelGetText(PX_Object *Label);
px_void		PX_Object_LabelSetText(PX_Object *pLabel,px_char *Text);
px_void		PX_Object_LabelSetTextColor(PX_Object *pLabel,px_color Color);
px_void		PX_Object_LabelSetBackgroundColor(PX_Object *pLabel,px_color Color);
px_void		PX_Object_LabelSetAlign(PX_Object *pLabel,px_uchar Align);
px_void		PX_Object_LabelSetBorder(PX_Object *pLabel,px_bool Border);
px_void		PX_Object_LabelRender(px_surface *psurface,PX_Object *pLabel,px_uint elpased);    
px_void		PX_Object_LabelFree(PX_Object *pLabel);

PX_Object *	PX_Object_ProcessBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height);
PX_Object_ProcessBar *PX_Object_GetProcessBar(PX_Object *Object);
px_void		PX_Object_ProcessBarSetColor(PX_Object *pProcessBar,px_color Color);
px_void		PX_Object_ProcessBarSetValue(PX_Object *pProcessBar,px_int Value);
px_void		PX_Object_ProcessBarSetMax(PX_Object *pProcessBar,px_int Max);
px_void		PX_Object_ProcessBarRender(px_surface *psurface,PX_Object *pProcessBar,px_uint elpased);
px_int		PX_Object_ProcessBarGetValue(PX_Object *pProcessBar);
px_void		PX_Object_ProcessBarFree(PX_Object *pProcessBar);

PX_Object * PX_Object_RadiusICOCreate(px_memorypool *mp,PX_Object*Parent,px_int x,px_int y,px_int Radius);
PX_Object_RadiusICO *PX_Object_GetRadiusICO(PX_Object *Object);
px_void		PX_Object_RadiusICOSetColor(PX_Object *pRadiusICO,px_color Color);
px_void		PX_Object_RadiusICORender(px_surface *psurface,PX_Object *pRadiusICO,px_uint elpased);
px_void     PX_Object_RadiusICOFree(PX_Object *pRadiusICO);

PX_Object *PX_Object_ImageCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_texture *ptex);
PX_Object_Image *PX_Object_GetImage(PX_Object *Object);
px_void	   PX_Object_ImageSetAlign(PX_Object *pImage,px_uchar Align);
px_void	   PX_Object_ImageSetMask(PX_Object *pImage,px_texture *pmask);
px_void	   PX_Object_ImageRender(px_surface *psurface,PX_Object *pImage,px_uint elpased);
px_void	   PX_Object_ImageFree(PX_Object *pImage);
px_void	   PX_Object_ImageFreeWithTexture(PX_Object *pImage);

PX_Object *PX_Object_SliderBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,enum PX_OBJECT_SLIDERBAR_TYPE Type,enum PX_OBJECT_SLIDERBAR_STYLE style);
PX_Object_SliderBar *PX_Object_GetSliderBar(PX_Object *Object);
px_void	   PX_Object_SliderBarSetValue(PX_Object *pSliderBar,px_int Value);
px_void	   PX_Object_SliderBarSetMax(PX_Object *pSliderBar,px_int Max);
px_int	   PX_Object_SliderBarGetValue(PX_Object *pSliderBar);
px_void	   PX_Object_SliderBarRender(px_surface *psurface,PX_Object *pSliderBar,px_uint elpased);
px_void    PX_Object_SliderBarSetBackgroundColor(PX_Object *pSliderBar,px_color color);
px_void	   PX_Object_SliderBarFree(PX_Object *pSliderBar);
px_void    PX_Object_SliderBarSetColor(PX_Object *pSliderBar,px_color color);
px_void	   PX_Object_SliderBarSetSliderButtonLength(PX_Object *pSliderBar,px_int length);




PX_Object *PX_Object_PushButtonCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_char *Text,px_color Color);
PX_Object_PushButton * PX_Object_GetPushButton( PX_Object *Object );
px_char * PX_Object_PushButtonGetText( PX_Object *PushButton );
px_void PX_Object_PushButtonSetText( PX_Object *pObject,px_char *Text );
px_void PX_Object_PushButtonSetBackgroundColor( PX_Object *pObject,px_color Color );
px_void PX_Object_PushButtonSetCursorColor( PX_Object *pObject,px_color Color );
px_void PX_Object_PushButtonSetPushColor( PX_Object *pObject,px_color Color );
px_void PX_Object_PushButtonSetBorderColor( PX_Object *pObject,px_color Color );
px_void PX_Object_PushButtonSetAlign( PX_Object *pObject,px_uchar Align );
px_void PX_Object_PushButtonSetBorder( PX_Object *Object,px_uchar Border );
px_void PX_Object_PushButtonRender(px_surface *psurface, PX_Object *pObject,px_uint elpased);
px_void PX_Object_PushButtonSetTextColor( PX_Object *pObject,px_color Color );
px_void PX_Object_PushButtonSetImage(PX_Object *pObject,px_texture *texture);
px_void PX_Object_PushButtonFree( PX_Object *Obj );

PX_Object* PX_Object_EditCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_color TextColor );
PX_Object_Edit * PX_Object_GetEdit( PX_Object *Object );
px_char * PX_Object_EditGetText( PX_Object *pObject );
px_void PX_Object_EditSetMaxTextLength(PX_Object *pObject,px_int max_length);
px_void PX_Object_EditSetText( PX_Object *pObject,px_char *Text );
px_void PX_Object_EditSetFocus( PX_Object *pObject);
px_void PX_Object_EditSetPasswordStyle( PX_Object *pObject,px_uchar Enabled );
px_void PX_Object_EditSetBackgroundColor( PX_Object *pObject,px_color Color );
px_void PX_Object_EditSetBorderColor( PX_Object *pObject,px_color Color );
px_void PX_Object_EditSetCursorColor( PX_Object *pObject,px_color Color );
px_void PX_Object_EditSetTextColor( PX_Object *pObject,px_color Color );
px_void PX_Object_EditSetBorder( PX_Object *pObj,px_uchar Border );
px_void PX_Object_EditRender(px_surface *psurface, PX_Object *pObject,px_uint elpased);
px_void PX_Object_EditFree( PX_Object *pObject );
px_void PX_Object_EditAddString(PX_Object *pObject,px_char *Text);
px_void PX_Object_EditBackspace(PX_Object *pObject);
px_void PX_Object_EditAutoNewLine(PX_Object *pObject,px_bool b);
px_void PX_Object_EditSetOffset(PX_Object *pObject,px_int TopOffset,px_int LeftOffset);
px_void PX_Object_EditCursorBack(PX_Object *pObject);
px_void PX_Object_EditCursorForward(PX_Object *pObject);

// PX_Object * PX_Object_StaticImageCreate( PX_Object *Parent,px_int x,px_int y,px_texture tex);
// PX_Object_StaticImage * PX_Object_GetStaticImage( PX_Object *Object );
// px_void PX_Object_StaticImageSetAlign( PX_Object *pImage,px_uchar Align);
// px_void PX_Object_StaticImageRender(px_surface *psurface, PX_Object *im,px_uint elpased);
// px_void PX_Object_StaticImageFree(PX_Object *pObj);


PX_Object *PX_Object_ScrollAreaCreate(px_memorypool *mp,PX_Object *Parent,int x,int y,int height,int width);
PX_Object_ScrollArea * PX_Object_GetScrollArea( PX_Object *Object );
PX_Object * PX_Object_ScrollAreaGetIncludedObjects(PX_Object *pObj);
px_void PX_Object_ScrollAreaMoveToBottom(PX_Object *pObject);
px_void PX_Object_ScrollAreaMoveToTop(PX_Object *pObject);
px_void PX_Object_ScrollAreaGetWidthHeight(PX_Object *pObject,px_int *Width,px_int *Height);
px_void PX_Object_ScrollAreaRender(px_surface *psurface, PX_Object *pObject,px_uint elpased);
px_void PX_Object_ScrollAreaSetBkColor(PX_Object *pObj,px_color bkColor);
px_void PX_Object_ScrollAreaSetBorder( PX_Object *pObj,px_uchar Border );
px_void PX_Object_ScrollAreaSetBorderColor(PX_Object *pObj,px_color borderColor);
px_void PX_Object_ScrollAreaFree(PX_Object *pObj);


PX_Object *PX_Object_RotationCreate(px_memorypool *mp,PX_Object *Parent,px_int angle_per_second,px_int x,px_int y,px_texture *ptexture);
PX_Object_Rotation * PX_Object_GetRotation( PX_Object *Object );
px_void PX_Object_RotationSetSpeed(PX_Object *rot,px_int Angle_per_second);
px_void PX_Object_RotationRender(px_surface *psurface, PX_Object *Obj,px_uint elpased);

PX_Object *PX_Object_AutoTextCreate(px_memorypool *mp,PX_Object *Parent,int x,int y,int limit_width);
PX_Object_AutoText * PX_Object_GetAutoText( PX_Object *Object );
px_void PX_Object_AutoTextSetTextColor( PX_Object *pObject,px_color Color );
px_void PX_Object_AutoTextSetText(PX_Object *Obj,px_char *Text);
px_void PX_Object_AutoTextRender(px_surface *psurface, PX_Object *pObject,px_uint elpased);
px_void PX_Object_AutoTextFree(PX_Object *Obj);
px_int PX_Object_AutoTextGetHeight(PX_Object *Obj);


PX_Object *PX_Object_AnimationCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_animationlibrary *lib);
px_void PX_Object_AnimationSetLibrary(PX_Object *Object,px_animationlibrary *lib);
PX_Object_Animation *PX_Object_GetAnimation(PX_Object *Object);
px_void	   PX_Object_AnimationSetAlign(PX_Object *pObject,px_uchar Align);
px_void	   PX_Object_AnimationRender(px_surface *psurface,PX_Object *pImage,px_uint elpased);
px_void	   PX_Object_AnimationFree(PX_Object *pObject);


PX_Object *PX_Object_ParticalCreateEx(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,PX_ParticalLauncher_InitializeInfo info);
PX_Object *PX_Object_ParticalCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_texture *pTexture,PX_ScriptVM_Instance *pIns,px_char *_init,px_char *_create,px_char *_update);
PX_Object_Partical *PX_Object_GetPartical(PX_Object *Object);
px_void    PX_Object_ParticalSetDirection(PX_Object *pObject,px_point direction);
px_void	   PX_Object_ParticalRender(px_surface *psurface,PX_Object *pObject,px_uint elpased);
px_void	   PX_Object_ParticalFree(PX_Object *pObject);


PX_Object *PX_Object_RoundCursorCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_shape *pShape,px_color blendColor);
PX_Object_RoundCursor *PX_Object_GetRoundCursor(PX_Object *Object);
px_void    PX_Object_RoundCursorSetDirection(PX_Object *Object,px_point cursor_point);
px_void	   PX_Object_RoundCursorRender(px_surface *psurface,PX_Object *pObject,px_uint elpased);
px_void PX_Object_RoundCursorSetColor(PX_Object *Object,px_color clr);
px_void		PX_Object_RoundCursorFree(PX_Object *pObject);

PX_Object * PX_Object_ShapeCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_shape *pshape );
PX_Object_Shape * PX_Object_GetShape( PX_Object *Object );
px_void PX_Object_ShapeSetAlign( PX_Object *pShape,px_uchar Align);
px_void PX_Object_ShapeSetBlendColor( PX_Object *pShape,px_color blendcolor);
px_void PX_Object_ShapeRender(px_surface *psurface, PX_Object *im,px_uint elpased);
px_void PX_Object_RoundCursor_Mousemove(PX_Object *pobject,PX_Object_Event e,px_void *user_ptr);

//use pushbutton function to operate cursor-button
PX_Object *PX_Object_CursorButtonCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,px_char *Text,px_color Color);
PX_Object_CursorButton * PX_Object_GetCursorButton( PX_Object *Object );
PX_Object * PX_Object_GetCursorButtonPushButton(PX_Object *Object);

#endif