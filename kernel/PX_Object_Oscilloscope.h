#ifndef PX_OBJECT_OSCILLOSCOPE_H
#define PX_OBJECT_OSCILLOSCOPE_H
#include "PX_Object.h"

#define  PX_OBJECT_OSCILLOSCOPEDATA_MAP_LEFT  0
#define  PX_OBJECT_OSCILLOSCOPEDATA_MAP_RIGHT 1
#define  PX_OBJECT_OSCILLOSCOPEDATA_MAP_HORIZONTAL 2

#define  PX_OBJECT_OSCILLOSCOPE_DEFAULE_MINHORIZONTALPIXELDIVIDING 48
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULE_MINVERTICALPIXELDIVIDING	  20
#define	 PX_OBJECT_OSCILLOSCOPE_DEFAULE_DIVIDING					  10
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_SPACER					  64
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_VERTICALFLAG_OFFSET        8
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLAGTEXT_SPACER		      8

#define	 PX_OBJECT_OSCILLOSCOPE_DEFAULT_FRAMELINE_WIDTH			  2
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_FONT_SIZE				  16
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_DASH_RGB					  255,100,100,255
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_LINE_WIDTH				  2.0f

#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_EXPONENTIAL_FORMAT	     "%1.2e%2"
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLOAT_FLAGFORMAT_H		 "%1.2"
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_INT_FLAGFORMAT_H			 "%1"

#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLOAT_FLAGFORMAT_L		 "%1.2"
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_INT_FLAGFORMAT_L			 "%1"

#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_FLOAT_FLAGFORMAT_R		 "%1.2"
#define  PX_OBJECT_OSCILLOSCOPE_DEFAULT_INT_FLAGFORMAT_R			 "%1"


#define  PX_OBJECT_OSCILLOSCOPEFLAGLINE_XSHOW						 1
#define  PX_OBJECT_OSCILLOSCOPEFLAGLINE_YLSHOW					 2
#define  PX_OBJECT_OSCILLOSCOPEFLAGLINE_YRSHOW					 4

typedef enum 
{
	PX_OBJECT_OSCILLOSCOPE_LINEMODE_LINES=0,
	PX_OBJECT_OSCILLOSCOPE_LINEMODE_PILLAR=1,
}PX_OBJECT_OSCILLOSCOPE_LINEMODE;

typedef enum 
{
	PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE_ALL=0,
	PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE_HORIZONTAL=1,
	PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE_VERTICAL=2
}PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE;

typedef enum
{
	PX_OBJECT_OSCILLOSCOPE_OSCILLOSCOPEDATA_MAP_LEFT,
	PX_OBJECT_OSCILLOSCOPE_OSCILLOSCOPEDATA_MAP_RIGHT,
}PX_OBJECT_OSCILLOSCOPE_OSCILLOSCOPEDATA_MAP;

typedef enum
{
	PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE_NORMAL,
	PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE_EXPONENTRAL,
}PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE;


typedef struct  
{
	px_double	X,Y;
	px_color	color;
	px_float	LineWidth;
	px_uchar    XYshow;
}PX_Object_OscilloscopeFlagLine;

typedef struct 
{
	px_double *MapHorizontalArray;
	px_double *MapVerticalArray;
	px_double Normalization;
	px_color  Color;
	PX_OBJECT_OSCILLOSCOPE_OSCILLOSCOPEDATA_MAP Map;
	px_int	  linewidth;
	px_int    ID;
	px_int	  Visibled;
	px_int	  Size;
}PX_Object_OscilloscopeData;


typedef struct
{
	px_memorypool *mp;
	px_double HorizontalRangeMin;
	px_double HorizontalRangeMax;
	px_double LeftVerticalRangeMin,LeftVerticalRangeMax;
	px_double RightVerticalRangeMin,RightVerticalRangeMax;
	PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE leftTextDisplayMode,RightTextDisplayMode;

	px_double ResHorizontalRangeMin;
	px_double ResHorizontalRangeMax;
	px_double ResLeftVerticalRangeMin,ResLeftVerticalRangeMax;
	px_double ResRightVerticalRangeMin,ResRightVerticalRangeMax;


	px_int MinVerticalPixelDividing;
	px_int MinHorizontalPixelDividing;
	px_int HorizontalDividing;
	px_int LeftVerticalDividing;
	px_int RightVerticalDividing;

	px_int LeftSpacer,RightSpacer,TopSpacer,BottomSpacer;

	const px_char *Exponential_Format;
	const px_char *FloatFlagFormat_H;
	const px_char *IntFlagFormat_H;
	const px_char *FloatFlagFormat_L;
	const px_char *IntFlagFormat_L;
	const px_char *FloatFlagFormat_R;
	const px_char *IntFlagFormat_R;
	const px_char *LeftTitle,*RightTitle,*TopTitle,*BottomTitle;

	px_point DragStartPoint;
	px_point DragingPoint;
	px_int bScaleDrag;
	px_int bScaling;
	px_int MarkLineX;

	px_bool ScaleEnabled;
	px_bool ShowGuides;
	px_bool MarkValueEnabled;
	px_bool OnMarkStatus;
	px_bool bDataUpdatePainter;
	px_bool ShowHelpLine;

	px_bool LeftTextShow,RightTextShow,HorizontalTextShow;

	px_color DashColor;
	px_color FontColor;
	px_color borderColor;
	px_color helpLineColor;
	int      FontSize;

	PX_OBJECT_OSCILLOSCOPE_LINEMODE LineMode;
	PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE guidesShowMode;

	px_float DataLineWidth;
	px_float DataPillarWidth;

	px_float GuidesLineWidth;
	px_vector  vData;
	px_vector  vFlagLine;

	px_int  helpLineX,helpLineY;
	PX_FontModule *fontmodule;
}PX_Object_Oscilloscope;


PX_Object_Oscilloscope *PX_Object_GetOscilloscope(PX_Object *pObject);
// 
px_void PX_Object_OscilloscopeSetMinVerticalPixelDividing(PX_Object *pObject,int val);
px_void PX_Object_OscilloscopeSetMinHorizontalPixelDividing(PX_Object *pObject,int val);
px_void PX_Object_OscilloscopeSetHorizontalDividing(PX_Object *pObject,int Count);
px_void PX_Object_OscilloscopeSetLeftVerticalDividing(PX_Object *pObject,int Count);
px_void PX_Object_OscilloscopeSetRightVerticalDividing(PX_Object *pObject,int Count);
px_void PX_Object_OscilloscopeSetStyle(PX_Object *pObject,PX_OBJECT_OSCILLOSCOPE_LINEMODE mode);
px_void PX_Object_OscilloscopeSetScaleEnabled(PX_Object *pObject,px_bool Enabled);
px_void PX_Object_OscilloscopeSetGuidesVisible(PX_Object *pObject,px_bool Visible); 
px_void PX_Object_OscilloscopeSetGuidesShowMode(PX_Object *pObject,PX_OBJECT_OSCILLOSCOPE_GUIDESSHOWMODE mode);
px_void PX_Object_OscilloscopeShowHelpLine(PX_Object *pObject,px_bool show); 

px_void PX_Object_OscilloscopeSetDataLineWidth(PX_Object *pObject,px_float linewidth );
px_void PX_Object_OscilloscopeSetDataShow(PX_Object *pObject,px_int index,px_bool show );
px_void PX_Object_OscilloscopeSetGuidesLineWidth(PX_Object *pObject,px_float linewidth);
px_void PX_Object_OscilloscopeSetGuidesLineColor(PX_Object *pObject,px_color clr);
px_void PX_Object_OscilloscopeSetTitleFontSize(PX_Object *pObject,int size);
px_void PX_Object_OscilloscopeSetTitleFontColor(PX_Object *pObject,px_color clr);
px_void PX_Object_OscilloscopeSetDashLineColor(PX_Object *pObject,px_color clr);
px_void PX_Object_OscilloscopeSetLeftTextShow(PX_Object *pObject,px_bool bshow);
px_void PX_Object_OscilloscopeSetRightTextShow(PX_Object *pObject,px_bool bshow);
px_void PX_Object_OscilloscopeSetHorizontalTextShow(PX_Object *pObject,px_bool bshow);
px_void PX_Object_OscilloscopeSetFloatFlagFormatHorizontal(PX_Object *pObject,const px_char *fmt);
px_void PX_Object_OscilloscopeSetIntFlagFormatHorizontal(PX_Object *pObject,const px_char *fmt);
px_void PX_Object_OscilloscopeSetFloatFlagFormatVerticalLeft(PX_Object *pObject,const px_char *fmt);
px_void PX_Object_OscilloscopeSetIntFlagFormatVerticalLeft(PX_Object *pObject,const px_char *fmt);
px_void PX_Object_OscilloscopeSetFloatFlagFormatVerticalRight(PX_Object *pObject,const px_char *fmt);
px_void PX_Object_OscilloscopeSetIntFlagFormatVericalRight(PX_Object *pObject,const px_char *fmt);


px_void PX_Object_OscilloscopeSetLeftTextMode(PX_Object *pObject,PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE mode);
px_void PX_Object_OscilloscopeSetRightTextMode(PX_Object *pObject,PX_OBJECT_OSCILLOSCOPE_TEXT_DISPLAYMODE mode);
px_void PX_Object_OscilloscopeSetHorizontalMin(PX_Object *pObject,double Min);
px_void PX_Object_OscilloscopeSetHorizontalMax(PX_Object *pObject,double Max);
px_void PX_Object_OscilloscopeSetLeftVerticalMin(PX_Object *pObject,double Min);
px_void PX_Object_OscilloscopeSetLeftVerticalMax(PX_Object *pObject,double Max);
px_void PX_Object_OscilloscopeSetRightVerticalMax(PX_Object *pObject,double Max);
px_void PX_Object_OscilloscopeSetRightVerticalMin(PX_Object *pObject,double Min);

px_void PX_Object_OscilloscopeSetBorderColor(PX_Object *pObject,px_color clr);
PX_Object_OscilloscopeData *PX_Object_OscilloscopeGetOscilloscopeData(PX_Object *pObject,px_int index);
int PX_Object_OscilloscopeGetOscilloscopeWidth(PX_Object *pObject);
int PX_Object_OscilloscopeGetOscilloscopeHeight(PX_Object *pObject);
//px_void PX_Object_OscilloscopeSetTitleLeft(PX_Object *pObject,const px_char * title);
//px_void PX_Object_OscilloscopeSetTitleRight(PX_Object *pObject,const px_char * title);
px_void PX_Object_OscilloscopeSetTitleTop(PX_Object *pObject,const px_char * title);
px_void PX_Object_OscilloscopeSetTitleBottom(PX_Object *pObject,const px_char * title);


px_void PX_Object_OscilloscopeSetMarkValueEnabled(PX_Object *pObject,px_bool Enabled);
px_void PX_Object_OscilloscopeSetFontColor(PX_Object *pObject,px_color clr);
px_void PX_Object_OscilloscopeClearContext(PX_Object *pObject);
px_void PX_Object_OscilloscopeClearFlagLine(PX_Object *pObject);
px_void PX_Object_OscilloscopeAddData(PX_Object *pObject,PX_Object_OscilloscopeData data);
px_void PX_Object_OscilloscopeClearData(PX_Object* pObject);
px_void PX_Object_OscilloscopeAddOscilloscopeFlagLine(PX_Object *pObject,PX_Object_OscilloscopeFlagLine Line);
// 
px_void PX_Object_OscilloscopeSetMargin(PX_Object *pObject,px_int Left,px_int Right,px_int Top,px_int Bottom);
px_void PX_Object_OscilloscopeRestoreOscilloscope(PX_Object *pObject);
PX_Object *PX_Object_OscilloscopeCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_FontModule *fontmodule);


#define PX_OBJECT_FILTER_EDITOR_MAX_PT 256
#define PX_OBJECT_FILTER_EDITOR_DEFAULT_RADIUS 6

typedef enum
{
	PX_OBJECT_FILTER_TYPE_Liner,
	PX_OBJECT_FILTER_TYPE_dB,
}PX_OBJECT_FILTER_TYPE;


typedef struct
{
	px_int x,y;
	px_bool bselect;
	px_bool bCursor;
}PX_Object_FilterEditor_OperatorPoint;


typedef struct
{
	px_bool ShowHelpLine;
	px_color FontColor;
	px_color borderColor;
	px_color helpLineColor;
	px_color ptColor;
	px_bool showHorizontal;
	int      FontSize;
	px_int   HorizontalDividing;
	px_int   VerticalDividing;
	px_point DragStartPoint;
	px_point lastAdjustPoint;
	px_point DragingPoint;
	px_int	 bSelectDrag;
	px_int	 bAdjust;
	px_int   opCount;
	px_int   radius;
	px_double rangedb;
	PX_OBJECT_FILTER_TYPE FilterType;
	PX_Object_FilterEditor_OperatorPoint pt[PX_OBJECT_FILTER_EDITOR_MAX_PT];
}PX_Object_FilterEditor;

PX_Object_FilterEditor *PX_Object_GetFilterEditor(PX_Object *Object);

PX_Object *PX_Object_FilterEditorCreate(px_memorypool *mp, PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_OBJECT_FILTER_TYPE type);
px_void PX_Object_FilterEditorSethelpLineColor(PX_Object *Object,px_color clr);
px_void PX_Object_FilterEditorSetptColor(PX_Object *Object,px_color clr);

px_void PX_Object_FilterEditorSetOperateCount(PX_Object *Object,px_int opcount);
px_void PX_Object_FilterEditorSetType(PX_Object *Object,PX_OBJECT_FILTER_TYPE type);
px_void PX_Object_FilterEditorSetHorizontalShow(PX_Object *Object,px_bool HorizontalShow);
px_void PX_Object_FilterEditorReset(PX_Object *Object);
px_void PX_Object_FilterEditorSetRange(PX_Object *Object,px_double range);
px_void PX_Object_FilterEditorSetFontColor(PX_Object *Object,px_color clr);
px_void PX_Object_FilterEditorSetBorderColor(PX_Object *Object,px_color clr);
px_void PX_Object_FilterEditorSethelpLineColor(PX_Object *Object,px_color clr);
px_void PX_Object_FilterEditorSetFontSize(PX_Object *Object,px_int size);
px_void PX_Object_FilterEditorSetHorizontalDividing(PX_Object *Object,px_int div);
px_void PX_Object_FilterEditorSetVerticalDividing(PX_Object *Object,px_int div);
px_void PX_Object_FilterEditorMapData(PX_Object *Object,px_double data[],px_int size);
px_double PX_Object_FilterEditorMapValue(PX_Object *Object,px_double precent);




#endif

