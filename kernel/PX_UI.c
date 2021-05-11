#include "PX_UI.h"

typedef enum
{
	PX_UI_HALIGN_LEFT,
	PX_UI_HALIGN_MID,
	PX_UI_HALIGN_RIGHT,
}PX_UI_HALIGN;

typedef enum
{
	PX_UI_VALIGN_TOP,
	PX_UI_VALIGN_MID,
	PX_UI_VALIGN_BOTTOM,
}PX_UI_VALIGN;

typedef struct
{
	px_char id[32];
	px_float x,y,z,height,width,length;
	PX_UI_HALIGN halign;
	PX_UI_VALIGN valign;
}PX_UiBaseInfo;

PX_UiBaseInfo PX_UIGetBaseInfo(PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_UiBaseInfo baseInfo;
	PX_Json_Value *pSubValue=PX_NULL;
	PX_memset(&baseInfo,0,sizeof(PX_UiBaseInfo));
	pSubValue=PX_JsonGetObjectValue(json_value,"x");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		baseInfo.x=(px_float)pSubValue->_number;
	}
	pSubValue=PX_JsonGetObjectValue(json_value,"y");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		baseInfo.y=(px_float)pSubValue->_number;
	}
	pSubValue=PX_JsonGetObjectValue(json_value,"z");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		baseInfo.z=(px_float)pSubValue->_number;
	}
	pSubValue=PX_JsonGetObjectValue(json_value,"width");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		baseInfo.width=(px_float)pSubValue->_number;
	}
	pSubValue=PX_JsonGetObjectValue(json_value,"height");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		baseInfo.height=(px_float)pSubValue->_number;
	}
	pSubValue=PX_JsonGetObjectValue(json_value,"length");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		baseInfo.length=(px_float)pSubValue->_number;
	}
	pSubValue=PX_JsonGetObjectValue(json_value,"id");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		PX_strcpy(baseInfo.id,pSubValue->_string.buffer,sizeof(baseInfo.id));
	}
	pSubValue=PX_JsonGetObjectValue(json_value,"halign");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		if(PX_strequ(pSubValue->_string.buffer,"left"))
		{
			//
		}
		if(PX_strequ(pSubValue->_string.buffer,"mid"))
		{
			baseInfo.x+=width/2;
		}
		if(PX_strequ(pSubValue->_string.buffer,"right"))
		{
			baseInfo.x+=width;
		}
	}

	pSubValue=PX_JsonGetObjectValue(json_value,"valign");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		if(PX_strequ(pSubValue->_string.buffer,"top"))
		{
			//
		}
		if(PX_strequ(pSubValue->_string.buffer,"mid"))
		{
			baseInfo.y+=height/2;
		}
		if(PX_strequ(pSubValue->_string.buffer,"bottom"))
		{
			baseInfo.y+=height;
		}
	}

	return baseInfo;
}

px_bool PX_UI_GetColor(PX_Json_Value *json_value,const px_char name[],px_color *color)
{
	PX_Json_Value *pSubValue=PX_NULL;
	pSubValue=PX_JsonGetObjectValue(json_value,name);
	if (pSubValue)
	{
		if (pSubValue->type==PX_JSON_VALUE_TYPE_ARRAY)
		{
			PX_Json_Value *parrayValue;
			parrayValue=PX_JsonGetArrayValue(pSubValue,0);
			if (parrayValue&&parrayValue->type==PX_JSON_VALUE_TYPE_NUMBER)
			{
				color->_argb.a=(px_byte)parrayValue->_number;
			}
			parrayValue=PX_JsonGetArrayValue(pSubValue,1);
			if (parrayValue&&parrayValue->type==PX_JSON_VALUE_TYPE_NUMBER)
			{
				color->_argb.r=(px_byte)parrayValue->_number;
			}
			parrayValue=PX_JsonGetArrayValue(pSubValue,2);
			if (parrayValue&&parrayValue->type==PX_JSON_VALUE_TYPE_NUMBER)
			{
				color->_argb.g=(px_byte)parrayValue->_number;
			}
			parrayValue=PX_JsonGetArrayValue(pSubValue,3);
			if (parrayValue&&parrayValue->type==PX_JSON_VALUE_TYPE_NUMBER)
			{
				color->_argb.b=(px_byte)parrayValue->_number;
			}
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_UI_GetBool(PX_Json_Value *json_value,const px_char name[],px_bool *b)
{
	PX_Json_Value *pSubValue=PX_NULL;
	pSubValue=PX_JsonGetObjectValue(json_value,name);
	if (pSubValue)
	{
		if (pSubValue->type==PX_JSON_VALUE_TYPE_BOOLEAN)
		{
			*b=pSubValue->_boolean;
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_UI_GetNumber(PX_Json_Value *json_value,const px_char name[],px_double *number)
{
	PX_Json_Value *pSubValue=PX_NULL;
	pSubValue=PX_JsonGetObjectValue(json_value,name);
	if (pSubValue)
	{
		if (pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
		{
			*number=pSubValue->_number;
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}
px_bool PX_UI_GetString(PX_Json_Value *json_value,const px_char name[],px_char str[],px_int size)
{
	PX_Json_Value *pSubValue=PX_NULL;
	pSubValue=PX_JsonGetObjectValue(json_value,name);
	if (pSubValue)
	{
		if (pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
		{
			PX_strcpy(str,pSubValue->_string.buffer,size);
			return PX_TRUE;
		}
	}
	return PX_FALSE;
}

PX_Object * PX_UI_CreateNode(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	return PX_ObjectCreate(ui->ui_mp,parent,0,0,0,0,0,0);
}

PX_Object * PX_UI_CreateLabel(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	const px_char *text="";
	px_char style[8];
	px_bool border;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color fontColor,backgroundColor,Color;
	pSubValue=PX_JsonGetObjectValue(json_value,"text");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		text=pSubValue->_string.buffer;
	}

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);
	fontColor=PX_COLOR(255,0,0,0);
	backgroundColor=PX_COLOR(0,0,0,0);

	PX_UI_GetColor(json_value,"fontcolor",&fontColor);
	PX_UI_GetColor(json_value,"backgroundcolor",&backgroundColor);
	pObject=PX_Object_LabelCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,text,ui->fontmodule,fontColor);
	PX_Object_LabelSetBackgroundColor(pObject,backgroundColor);

	if (PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_LabelSetBorderColor(pObject,Color);
	}

	if (PX_UI_GetString(json_value,"style",style,sizeof(style)))
	{
		if (PX_strequ(style,"round"))
		{
			PX_Object_LabelSetStyle(pObject,PX_OBJECT_LABEL_STYLE_ROUNDRECT);
		}
	}

	if (PX_UI_GetBool(json_value,"border",&border))
	{
		PX_Object_LabelSetBorder(pObject,border);
	}

	pSubValue=PX_JsonGetObjectValue(json_value,"align");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		if (PX_strequ(pSubValue->_string.buffer,"lefttop"))
		{
			PX_Object_LabelSetAlign(pObject,PX_ALIGN_LEFTTOP);
		}
		else if (PX_strequ(pSubValue->_string.buffer,"leftmid"))
		{
			PX_Object_LabelSetAlign(pObject,PX_ALIGN_LEFTMID);
		}
		else if (PX_strequ(pSubValue->_string.buffer,"leftbottom"))
		{
			PX_Object_LabelSetAlign(pObject,PX_ALIGN_LEFTBOTTOM);
		}
		else if (PX_strequ(pSubValue->_string.buffer,"midtop"))
		{
			PX_Object_LabelSetAlign(pObject,PX_ALIGN_MIDTOP);
		}
		else if (PX_strequ(pSubValue->_string.buffer,"center"))
		{
			PX_Object_LabelSetAlign(pObject,PX_ALIGN_CENTER);
		}
		else if (PX_strequ(pSubValue->_string.buffer,"midbottom"))
		{
			PX_Object_LabelSetAlign(pObject,PX_ALIGN_MIDBOTTOM);
		}
		else if (PX_strequ(pSubValue->_string.buffer,"righttop"))
		{
			PX_Object_LabelSetAlign(pObject,PX_ALIGN_RIGHTTOP);
		}
		else if (PX_strequ(pSubValue->_string.buffer,"rightmid"))
		{
			PX_Object_LabelSetAlign(pObject,PX_ALIGN_RIGHTMID);
		}
		else if (PX_strequ(pSubValue->_string.buffer,"rightbottom"))
		{
			PX_Object_LabelSetAlign(pObject,PX_ALIGN_RIGHTBOTTOM);
		}
	}

	return pObject;
}
PX_Object * PX_UI_CreateProcessbar(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	px_int max=100;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color,backgroundColor;
	
	baseInfo=PX_UIGetBaseInfo(json_value,width,height);
	Color=PX_COLOR(255,0,0,0);
	backgroundColor=PX_COLOR(0,0,0,0);

	pObject=PX_Object_ProcessBarCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height);

	pSubValue=PX_JsonGetObjectValue(json_value,"max");

	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		max=(px_int)pSubValue->_number;
		PX_Object_ProcessBarSetMax(pObject,max);
	}

	if(PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_ProcessBarSetColor(pObject,Color);
	}

	if(PX_UI_GetColor(json_value,"backgroundcolor",&backgroundColor))
	{
		PX_Object_ProcessBarSetBackgroundColor(pObject,backgroundColor);
	}
	

	return pObject;
}
PX_Object * PX_UI_CreateImage(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	px_int max=100;
	PX_Json_Value *pSubValue=PX_NULL;

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);

	pObject=PX_Object_ImageCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,PX_NULL);

	return pObject;
}
PX_Object * PX_UI_CreateSliderbar(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	px_int max=100;
	px_int min=0;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color,backgroundColor;
	PX_OBJECT_SLIDERBAR_TYPE type=PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL;
	PX_OBJECT_SLIDERBAR_STYLE style=PX_OBJECT_SLIDERBAR_STYLE_BOX;

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);

	pSubValue=PX_JsonGetObjectValue(json_value,"max");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		max=(px_int)pSubValue->_number;
	}

	pSubValue=PX_JsonGetObjectValue(json_value,"min");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		min=(px_int)pSubValue->_number;
	}

	pSubValue=PX_JsonGetObjectValue(json_value,"type");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING&&PX_strequ(pSubValue->_string.buffer,"vertical"))
	{
		type=PX_OBJECT_SLIDERBAR_TYPE_VERTICAL;
	}

	pSubValue=PX_JsonGetObjectValue(json_value,"style");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING&&PX_strequ(pSubValue->_string.buffer,"liner"))
	{
		style=PX_OBJECT_SLIDERBAR_STYLE_LINER;
	}

	pObject=PX_Object_SliderBarCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,type,style);
	if (pObject)
	{
		PX_Object_SliderBarSetRange(pObject,min,max);

		if(PX_UI_GetColor(json_value,"color",&Color))
		{
			PX_Object_SliderBarSetColor(pObject,Color);
		}

		if(PX_UI_GetColor(json_value,"backgroundcolor",&backgroundColor))
		{
			PX_Object_SliderBarSetBackgroundColor(pObject,backgroundColor);
		}

		pSubValue=PX_JsonGetObjectValue(json_value,"buttonlength");
		if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
		{
			PX_Object_SliderBarSetSliderButtonLength(pObject,(px_int)pSubValue->_number);
		}
	}

	return pObject;
}
PX_Object * PX_UI_CreateButton(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	const px_char *text="";
	px_char style[32];
	px_bool border;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color fontColor,Color;
	pSubValue=PX_JsonGetObjectValue(json_value,"text");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		text=pSubValue->_string.buffer;
	}

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);
	fontColor=PX_OBJECT_UI_DEFAULT_FONTCOLOR;

	PX_UI_GetColor(json_value,"fontcolor",&fontColor);

	pObject=PX_Object_PushButtonCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,text,ui->fontmodule);

	if (PX_UI_GetColor(json_value,"backgroundcolor",&Color))
	{
		PX_Object_PushButtonSetBackgroundColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"cursorcolor",&Color))
	{
		PX_Object_PushButtonSetCursorColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"pushcolor",&Color))
	{
		PX_Object_PushButtonSetPushColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_PushButtonSetBorderColor(pObject,Color);
	}

	if (PX_UI_GetString(json_value,"style",style,sizeof(style)))
	{
		if (PX_strequ(style,"round"))
		{
			PX_Object_PushButtonSetStyle(pObject,PX_OBJECT_PUSHBUTTON_STYLE_ROUNDRECT);
		}
	}

	if (PX_UI_GetBool(json_value,"border",&border))
	{
		PX_Object_PushButtonSetBorder(pObject,border);
	}
	return pObject;
}
PX_Object * PX_UI_CreateEdit(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	const px_char *text="";
	px_bool b;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color;
	px_char style[8];
	px_char str[128];
	px_double number;

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);
	
	pObject=PX_Object_EditCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,ui->fontmodule);

	if (PX_UI_GetColor(json_value,"fontcolor",&Color))
	{
		PX_Object_EditSetTextColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"backgroundcolor",&Color))
	{
		PX_Object_EditSetBackgroundColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"cursorcolor",&Color))
	{
		PX_Object_EditSetCursorColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_EditSetBorderColor(pObject,Color);
	}

	if (PX_UI_GetBool(json_value,"border",&b))
	{
		PX_Object_EditSetBorder(pObject,b);
	}

	if (PX_UI_GetBool(json_value,"passwordstyle",&b))
	{
		PX_Object_EditSetPasswordStyle(pObject,b);
	}

	if (PX_UI_GetBool(json_value,"autonewline",&b))
	{
		PX_Object_EditAutoNewLine(pObject,b,16);
	}

	if (PX_UI_GetString(json_value,"limit",str,sizeof(str)))
	{
		PX_Object_EditSetLimit(pObject,str);
	}

	if (PX_UI_GetNumber(json_value,"maxlength",&number))
	{
		PX_Object_EditSetMaxTextLength(pObject,(px_int)number);
	}

	if (PX_UI_GetString(json_value,"text",str,sizeof(str)))
	{
		PX_Object_EditSetText(pObject,str);
	}

	if (PX_UI_GetString(json_value,"style",style,sizeof(style)))
	{
		if (PX_strequ(style,"round"))
		{
			PX_Object_EditSetStyle(pObject,PX_OBJECT_EDIT_STYLE_ROUNDRECT);
		}
	}

	return pObject;
}
PX_Object * PX_UI_CreateScrollArea(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	px_bool b;
	px_color Color;


	baseInfo=PX_UIGetBaseInfo(json_value,width,height);

	pObject=PX_Object_ScrollAreaCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height);

	if (PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_EditSetBorderColor(pObject,Color);
	}

	if (PX_UI_GetBool(json_value,"border",&b))
	{
		PX_Object_EditSetBorder(pObject,b);
	}
	return pObject;
}
PX_Object * PX_UI_CreateAutoText(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	const px_char *text="";
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color;

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);

	pObject=PX_Object_AutoTextCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,ui->fontmodule);

	pSubValue=PX_JsonGetObjectValue(json_value,"text");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		text=pSubValue->_string.buffer;
		PX_Object_AutoTextSetText(pObject,text);
	}

	if (PX_UI_GetColor(json_value,"fontcolor",&Color))
	{
		PX_Object_AutoTextSetTextColor(pObject,Color);
	}

	return pObject;
}
PX_Object * PX_UI_CreateCursorButton(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	const px_char *text="";
	px_char style[32];
	px_bool border;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color fontColor,Color;
	pSubValue=PX_JsonGetObjectValue(json_value,"text");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		text=pSubValue->_string.buffer;
	}

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);
	fontColor=PX_COLOR(255,0,0,0);

	PX_UI_GetColor(json_value,"fontcolor",&fontColor);

	pObject=PX_Object_CursorButtonCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,text,ui->fontmodule,fontColor);

	if (PX_UI_GetColor(json_value,"backgroundcolor",&Color))
	{
		PX_Object_PushButtonSetBackgroundColor(PX_Object_GetCursorButton(pObject)->pushbutton,Color);
	}

	if (PX_UI_GetColor(json_value,"cursorcolor",&Color))
	{
		PX_Object_PushButtonSetCursorColor(PX_Object_GetCursorButton(pObject)->pushbutton,Color);
	}

	if (PX_UI_GetColor(json_value,"pushcolor",&Color))
	{
		PX_Object_PushButtonSetPushColor(PX_Object_GetCursorButton(pObject)->pushbutton,Color);
	}

	if (PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_PushButtonSetBorderColor(PX_Object_GetCursorButton(pObject)->pushbutton,Color);
	}

	if (PX_UI_GetString(json_value,"style",style,sizeof(style)))
	{
		if (PX_strequ(style,"round"))
		{
			PX_Object_PushButtonSetStyle(PX_Object_GetCursorButton(pObject)->pushbutton,PX_OBJECT_PUSHBUTTON_STYLE_ROUNDRECT);
		}
	}

	if (PX_UI_GetBool(json_value,"border",&border))
	{
		PX_Object_PushButtonSetBorder(PX_Object_GetCursorButton(pObject)->pushbutton,border);
	}
	return pObject;
}
PX_Object * PX_UI_CreateVirtualKeyboard(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color;

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);

	pObject=PX_Object_VirtualKeyBoardCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height);

	if (PX_UI_GetColor(json_value,"backgroundcolor",&Color))
	{
		PX_Object_VirtualKeyBoardSetBackgroundColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"cursorcolor",&Color))
	{
		PX_Object_VirtualKeyBoardSetCursorColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"pushcolor",&Color))
	{
		PX_Object_VirtualKeyBoardSetPushColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_VirtualKeyBoardSetBorderColor(pObject,Color);
	}

	return pObject;
}
PX_Object * PX_UI_CreateVirtualNumberKeyboard(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color;

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);

	pObject=PX_Object_VirtualNumberKeyBoardCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height);

	if (PX_UI_GetColor(json_value,"backgroundcolor",&Color))
	{
		PX_Object_VirtualNumberKeyBoardSetBackgroundColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"cursorcolor",&Color))
	{
		PX_Object_VirtualNumberKeyBoardSetCursorColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"pushcolor",&Color))
	{
		PX_Object_VirtualNumberKeyBoardSetPushColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_VirtualNumberKeyBoardSetBorderColor(pObject,Color);
	}

	return pObject;
}
PX_Object * PX_UI_CreateCheckBox(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	const px_char *text="";
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color;
	pSubValue=PX_JsonGetObjectValue(json_value,"text");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		text=pSubValue->_string.buffer;
	}

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);

	pObject=PX_Object_CheckBoxCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,text,ui->fontmodule);

	if (PX_UI_GetColor(json_value,"fontcolor",&Color))
	{
		PX_Object_CheckBoxSetTextColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"backgroundcolor",&Color))
	{
		PX_Object_CheckBoxSetBackgroundColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"cursorcolor",&Color))
	{
		PX_Object_CheckBoxSetCursorColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"pushcolor",&Color))
	{
		PX_Object_CheckBoxSetPushColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_CheckBoxSetBorderColor(pObject,Color);
	}

	return pObject;
}

px_bool PX_UI_IsValidUIObject(PX_UI *ui,PX_Json_Value *json_value)
{
	if (json_value->type==PX_JSON_VALUE_TYPE_OBJECT)
	{
		if (json_value->name.buffer&&json_value->name.buffer[0])
		{
			px_int i;
			for (i=0;i<ui->infos.size;i++)
			{
				PX_UI_ControllerInfo *pInfo=PX_VECTORAT(PX_UI_ControllerInfo,&ui->infos,i);
				if (PX_strequ(pInfo->Type,json_value->name.buffer))
				{
					return PX_TRUE;
				}
			}
		}
	}
	return PX_FALSE;
}

PX_Object * PX_UI_CreateRadioBox(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	const px_char *text="";
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color;
	px_bool bselect;
	pSubValue=PX_JsonGetObjectValue(json_value,"text");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		text=pSubValue->_string.buffer;
	}

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);

	pObject=PX_Object_RadioBoxCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,text,ui->fontmodule);

	if (PX_UI_GetColor(json_value,"fontcolor",&Color))
	{
		PX_Object_RadioBoxSetTextColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"backgroundcolor",&Color))
	{
		PX_Object_RadioBoxSetBackgroundColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"cursorcolor",&Color))
	{
		PX_Object_RadioBoxSetCursorColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"pushcolor",&Color))
	{
		PX_Object_RadioBoxSetPushColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_RadioBoxSetBorderColor(pObject,Color);
	}

	if (PX_UI_GetBool(json_value,"select",&bselect))
	{
		PX_Object_RadioBoxSetCheck(pObject,bselect);
	}

	if (PX_UI_GetBool(json_value,"check",&bselect))
	{
		PX_Object_RadioBoxSetCheck(pObject,bselect);
	}

	return pObject;
}
PX_Object * PX_UI_CreateSelectBar(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color;
	px_char style[8];
	px_double number;

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);

	pObject=PX_Object_SelectBarCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,ui->fontmodule);

	if (PX_UI_GetColor(json_value,"fontcolor",&Color))
	{
		PX_Object_SelectBarSetFontColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"backgroundcolor",&Color))
	{
		PX_Object_SelectBarSetBackgroundColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"cursorcolor",&Color))
	{
		PX_Object_SelectBarSetCursorColor(pObject,Color);
	}

	if (PX_UI_GetColor(json_value,"bordercolor",&Color))
	{
		PX_Object_SelectBarSetBorderColor(pObject,Color);
	}

	if (PX_UI_GetString(json_value,"style",style,sizeof(style)))
	{
		if (PX_strequ(style,"round"))
		{
			PX_Object_SelectBarSetStyle(pObject,PX_OBJECT_SELECTBAR_STYLE_ROUNDRECT);
		}
	}

	if (PX_UI_GetNumber(json_value,"displaycount",&number))
	{
		if (number<1)
		{
			number=1;
		}
		PX_Object_SelectBarSetDisplayCount(pObject,(px_int)number);
	}


	pSubValue=PX_JsonGetObjectValue(json_value,"items");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_ARRAY)
	{
		px_int i;
		PX_Json_Value *pArrayValue;
		for (i=0;i<pSubValue->_array.size;i++)
		{
			pArrayValue=PX_JsonGetArrayValue(pSubValue,i);
			if (pArrayValue&&pArrayValue->type==PX_JSON_VALUE_TYPE_STRING)
			{
				PX_Object_SelectBarAddItem(pObject,pArrayValue->_string.buffer);
			}
		}
	}

	if (PX_UI_GetNumber(json_value,"currentindex",&number))
	{
		PX_Object_SelectBarSetCurrentIndex(pObject,(px_int)number);
	}

	return pObject;
}

px_bool PX_UIInitialize(px_memorypool *mp,px_memorypool *ui_mp,PX_UI *ui,PX_FontModule *fontmodule)
{
	PX_memset(ui,0,sizeof(PX_UI));
	ui->mp=mp;
	ui->ui_mp=ui_mp;
	ui->fontmodule=fontmodule;
	if(!PX_VectorInitialize(mp,&ui->infos,sizeof(PX_UI_ControllerInfo),32)) return PX_FALSE;
	if(!PX_MapInitialize(mp,&ui->ObjectMap))
	{
		PX_VectorFree(&ui->infos);
		return PX_FALSE;
	}
	if(!PX_UIAddControllerInfo(ui,"node",PX_UI_CreateNode)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"label",PX_UI_CreateLabel)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"processbar",PX_UI_CreateProcessbar)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"image",PX_UI_CreateImage)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"sliderbar",PX_UI_CreateSliderbar)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"pushbutton",PX_UI_CreateButton)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"edit",PX_UI_CreateEdit)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"scrollarea",PX_UI_CreateScrollArea)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"text",PX_UI_CreateAutoText)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"cursorbutton",PX_UI_CreateCursorButton)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"virtualkeyboard",PX_UI_CreateVirtualKeyboard)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"virtualnumberkeyboard",PX_UI_CreateVirtualNumberKeyboard)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"checkbox",PX_UI_CreateCheckBox)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"radiobox",PX_UI_CreateRadioBox)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"selectbar",PX_UI_CreateSelectBar)) goto _ERROR;
	return PX_TRUE;
_ERROR:

	return PX_FALSE;
}

px_bool PX_UIAddControllerInfo(PX_UI *ui,const px_char controllertype[],PX_UI_ControllerCreate _func)
{
	PX_UI_ControllerInfo info;
	PX_strcpy(info.Type,controllertype,sizeof(info.Type));
	info.create_func=_func;
	return PX_VectorPushback(&ui->infos,&info);
}

PX_Object * PX_UICreate(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value,px_int width,px_int height)
{
	//key properties
	//type   default root 
	//x,y    default 0,0
	PX_Object *pNewObject=PX_NULL;
	PX_Json_Value *pSubValue=PX_NULL;

	if (json_value->type!=PX_JSON_VALUE_TYPE_OBJECT)
	{
		return PX_NULL;
	}
	
	if (json_value->name.buffer&&json_value->name.buffer[0])
	{
		px_int i;
		for (i=0;i<ui->infos.size;i++)
		{
			PX_UI_ControllerInfo *pInfo=PX_VECTORAT(PX_UI_ControllerInfo,&ui->infos,i);
			if (PX_strequ(pInfo->Type,json_value->name.buffer))
			{
				if (pInfo->create_func)
				{
					pNewObject=pInfo->create_func(ui,parent,json_value,width,height);
					if (!pNewObject)
					{
						goto _ERROR;
					}
					break;
				}
				else
				{
					goto _ERROR;
				}
			}
		}

	}
	else
	{
		//root object
		PX_UiBaseInfo baseInfo;
		baseInfo=PX_UIGetBaseInfo(json_value,width,height);

		pNewObject=PX_ObjectCreate(ui->ui_mp,parent,baseInfo.x,baseInfo.y,baseInfo.z,baseInfo.width,baseInfo.height,baseInfo.length);
		if (!pNewObject)
		{
			goto _ERROR;
		}
	}

	pSubValue=PX_JsonGetObjectValue(json_value,"id");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING&&pSubValue->_string.buffer[0])
	{
		if(PX_MapGet(&ui->ObjectMap,pSubValue->_string.buffer))
		{
			//"Repeat controller"
			PX_ASSERT();
			goto _ERROR;
		}
		if(PX_MapPut(&ui->ObjectMap,pSubValue->_string.buffer,pNewObject)!=PX_HASHMAP_RETURN_OK)
			goto _ERROR;
	}

	if (pNewObject)
	{
		px_list_node *pNode=PX_ListNodeAt(&json_value->_object.values,0);
		while (pNode)
		{
			PX_Json_Value *pvalue=PX_LIST_NODETDATA(PX_Json_Value,pNode);
			if (PX_UI_IsValidUIObject(ui,pvalue))
			{
				if(!PX_UICreate(ui,pNewObject,pvalue,width,height))
				{
					goto _ERROR;
				}
			}
			pNode=pNode->pnext;
		}
	}

	return pNewObject;
_ERROR:
	if (pNewObject)
	{
		PX_ObjectDelete(pNewObject);
	}
	return PX_NULL;

}

px_void PX_UIUpdateObjectsPostions(PX_UI *ui,PX_Object *pObject,PX_Json_Value *json_value,px_int width,px_int height)
{
	PX_UiBaseInfo baseInfo;
	PX_Json_Value *pSubValue=PX_NULL;
	PX_Object *pChildObject;
	px_int i;
	px_int index=0;

	baseInfo=PX_UIGetBaseInfo(json_value,width,height);
	pObject->x=baseInfo.x;
	pObject->y=baseInfo.y;
	pObject->Height=baseInfo.height;
	pObject->Width=baseInfo.width;
	if (json_value->type==PX_JSON_VALUE_TYPE_OBJECT)
	{
		for (i=0;i<json_value->_object.values.size;i++)
		{
			pSubValue=PX_JsonGetObjectValueByIndex(json_value,i);
			if (PX_UI_IsValidUIObject(ui,pSubValue))
			{
				pChildObject=PX_ObjectGetChild(pObject,index);
				if (pChildObject)
				{
					PX_UIUpdateObjectsPostions(ui,pChildObject,pSubValue,width,height);
				}
				index++;
			}
			
		}
	}
}

PX_Object * PX_UIGetObjectByID(PX_UI *ui,const px_char id[])
{
	return (PX_Object *)PX_MapGet(&ui->ObjectMap,id);
}

px_void PX_UIFree(PX_UI *ui)
{
	PX_VectorFree(&ui->infos);
	PX_MapFree(&ui->ObjectMap);
}

