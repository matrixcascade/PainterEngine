#ifndef PX_OBJECT_LABEL_H
#define PX_OBJECT_LABEL_H
#include "PX_Object.h"

typedef enum
{
	PX_OBJECT_LABEL_STYLE_RECT,
	PX_OBJECT_LABEL_STYLE_ROUNDRECT,
}PX_OBJECT_LABEL_STYLE;

typedef struct  
{
	PX_ALIGN Align;
	px_color borderColor;
	px_color TextColor;
	px_color BackgroundColor;
	px_bool  bBorder;
	PX_OBJECT_LABEL_STYLE style;
	PX_FontModule *fontModule;
	px_char *Text;
}PX_Object_Label;

PX_Object *	PX_Object_LabelCreate(px_memorypool *mp,PX_Object *Parent,px_int x,px_int y,px_int Width,px_int Height,const px_char *Text,PX_FontModule *fm,px_color Color);
PX_Object_Label  *	PX_Object_GetLabel(PX_Object *Object);
px_char	  * PX_Object_LabelGetText(PX_Object *Label);
px_void		PX_Object_LabelSetText(PX_Object *pLabel,const px_char *Text);
px_void		PX_Object_LabelSetTextColor(PX_Object *pLabel,px_color Color);
px_void		PX_Object_LabelSetBackgroundColor(PX_Object *pLabel,px_color Color);
px_void		PX_Object_LabelSetAlign(PX_Object *pLabel,PX_ALIGN Align);
px_void		PX_Object_LabelSetBorder(PX_Object *pLabel,px_bool b);
px_void		PX_Object_LabelSetBorderColor(PX_Object *pLabel,px_color color);
px_void		PX_Object_LabelSetStyle(PX_Object *pLabel,PX_OBJECT_LABEL_STYLE style);
px_int		PX_Object_LabelGetTextRenderWidth(PX_Object* pLabel);


#endif

