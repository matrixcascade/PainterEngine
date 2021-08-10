#ifndef PX_OBJECT_SLIDERBAR_H
#define PX_OBJECT_SLIDERBAR_H
#include "PX_Object.h"


typedef enum 
{
	PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL		,
	PX_OBJECT_SLIDERBAR_TYPE_VERTICAL		,
}PX_OBJECT_SLIDERBAR_TYPE;

typedef enum  
{
	PX_OBJECT_SLIDERBAR_STYLE_BOX			,
	PX_OBJECT_SLIDERBAR_STYLE_LINER			,
}PX_OBJECT_SLIDERBAR_STYLE;

typedef enum 
{
	PX_OBJECT_SLIDERBAR_STATUS_ONDRAG		,
	PX_OBJECT_SLIDERBAR_STATUS_NORMAL		,
}PX_OBJECT_SLIDERBAR_STATUS;


typedef struct 
{
	PX_OBJECT_SLIDERBAR_TYPE Type;
	PX_OBJECT_SLIDERBAR_STYLE style;
	PX_OBJECT_SLIDERBAR_STATUS status;
	px_float btnDownX,btnDownY;
	px_float DargButtonX,DargButtonY;
	px_int Min;
	px_int Max;
	px_int lastValue;
	px_int Value;
	px_int SliderButtonLength;
	px_color color;
	px_color BackgroundColor;
}PX_Object_SliderBar;

PX_Object *PX_Object_SliderBarCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,PX_OBJECT_SLIDERBAR_TYPE Type,PX_OBJECT_SLIDERBAR_STYLE style);
PX_Object_SliderBar *PX_Object_GetSliderBar(PX_Object *Object);
px_void	   PX_Object_SliderBarSetValue(PX_Object *pSliderBar,px_int Value);
px_void	   PX_Object_SliderBarSetRange(PX_Object *pSliderBar,px_int Min,px_int Max);
px_int	   PX_Object_SliderBarGetMax( PX_Object *pSliderBar );
px_int	   PX_Object_SliderBarGetValue(PX_Object *pSliderBar);
px_void	   PX_Object_SliderBarRender(px_surface *psurface,PX_Object *pSliderBar,px_uint elapsed);
px_void    PX_Object_SliderBarSetBackgroundColor(PX_Object *pSliderBar,px_color color);
px_void	   PX_Object_SliderBarFree(PX_Object *pSliderBar);
px_void    PX_Object_SliderBarSetColor(PX_Object *pSliderBar,px_color color);
px_void	   PX_Object_SliderBarSetSliderButtonLength(PX_Object *pSliderBar,px_int length);



#endif

