#include "PX_UI.h"

typedef struct
{
	px_char id[32];
	px_float x,y,z,height,width,length;
}PX_UiBaseInfo;

PX_UiBaseInfo PX_UIGetBaseInfo(PX_Json_Value *json_value)
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

PX_Object * PX_UI_CreateLabel(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	const px_char *text="";
	PX_Json_Value *pSubValue=PX_NULL;
	px_color fontColor,backgroundColor;
	pSubValue=PX_JsonGetObjectValue(json_value,"text");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_STRING)
	{
		text=pSubValue->_string.buffer;
	}

	baseInfo=PX_UIGetBaseInfo(json_value);
	fontColor=PX_COLOR(255,0,0,0);
	backgroundColor=PX_COLOR(0,0,0,0);

	PX_UI_GetColor(json_value,"fontcolor",&fontColor);
	PX_UI_GetColor(json_value,"backgroundcolor",&backgroundColor);
	pObject=PX_Object_LabelCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,text,ui->fontmodule,fontColor);
	PX_Object_LabelSetBackgroundColor(pObject,backgroundColor);

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
PX_Object * PX_UI_CreateProcessbar(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	px_int max=100;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color,backgroundColor;
	
	baseInfo=PX_UIGetBaseInfo(json_value);
	Color=PX_COLOR(255,0,0,0);
	backgroundColor=PX_COLOR(0,0,0,0);

	pObject=PX_Object_ProcessBarCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height);

	pSubValue=PX_JsonGetObjectValue(json_value,"max");

	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		max=(px_int)pSubValue->_number;
		PX_Object_ProcessBarSetMax(pObject,max);
	}

	if(PX_UI_GetColor(json_value,"color",&Color))
	{
		PX_Object_ProcessBarSetColor(pObject,Color);
	}

	if(PX_UI_GetColor(json_value,"backgroundcolor",&backgroundColor))
	{
		PX_Object_ProcessBarSetBackgroundColor(pObject,backgroundColor);
	}
	

	return pObject;
}
PX_Object * PX_UI_CreateImage(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	px_int max=100;
	PX_Json_Value *pSubValue=PX_NULL;

	baseInfo=PX_UIGetBaseInfo(json_value);

	pObject=PX_Object_ImageCreate(ui->ui_mp,parent,(px_int)baseInfo.x,(px_int)baseInfo.y,(px_int)baseInfo.width,(px_int)baseInfo.height,PX_NULL);

	return pObject;
}
PX_Object * PX_UI_CreateSliderbar(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value)
{
	PX_Object *pObject;
	PX_UiBaseInfo baseInfo;
	px_int max=100;
	px_int min=0;
	PX_Json_Value *pSubValue=PX_NULL;
	px_color Color,backgroundColor;
	PX_OBJECT_SLIDERBAR_TYPE type=PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL;
	PX_OBJECT_SLIDERBAR_STYLE style=PX_OBJECT_SLIDERBAR_STYLE_BOX;

	baseInfo=PX_UIGetBaseInfo(json_value);

	pSubValue=PX_JsonGetObjectValue(json_value,"max");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		max=(px_int)pSubValue->_number;
	}

	pSubValue=PX_JsonGetObjectValue(json_value,"min");
	if (pSubValue&&pSubValue->type==PX_JSON_VALUE_TYPE_NUMBER)
	{
		max=(px_int)pSubValue->_number;
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

	if(!PX_UIAddControllerInfo(ui,"label",PX_UI_CreateLabel)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"processbar",PX_UI_CreateProcessbar)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"image",PX_UI_CreateImage)) goto _ERROR;
	if(!PX_UIAddControllerInfo(ui,"sliderbar",PX_UI_CreateSliderbar)) goto _ERROR;
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

PX_Object * PX_UICreate(PX_UI *ui,PX_Object *parent,PX_Json_Value *json_value)
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
					pNewObject=pInfo->create_func(ui,parent,json_value);
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
		baseInfo=PX_UIGetBaseInfo(json_value);

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
			if (pvalue->type==PX_JSON_VALUE_TYPE_OBJECT)
			{
				if(!PX_UICreate(ui,pNewObject,pvalue))
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

PX_Object * PX_UIGetObjectByID(PX_UI *ui,const px_char id[])
{
	return (PX_Object *)PX_MapGet(&ui->ObjectMap,id);
}

px_void PX_UIFree(PX_UI *ui)
{
	PX_VectorFree(&ui->infos);
	PX_MapFree(&ui->ObjectMap);
}

