#include "PX_Object_Various.h"

PX_OBJECT_EVENT_FUNCTION(PX_Object_VariousOnEditChange)
{
	PX_Object* pVarObject = (PX_Object*)ptr;
	PX_ObjectExecuteEvent(pVarObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}


PX_OBJECT_RENDER_FUNCTION(PX_Object_VariousRender)
{
	PX_Object_Various* pDesc = PX_ObjectGetDesc(PX_Object_Various, pObject);

	pDesc->label->Width = pObject->Width * 0.25f;
	if (pDesc->label->Width<PX_Object_LabelGetTextRenderWidth(pDesc->label)+16)
	{
		pDesc->label->Width = PX_Object_LabelGetTextRenderWidth(pDesc->label) +16.f;
	}
	pDesc->label->Height = pObject->Height;

	


	pDesc->various->Width = pObject->Width -pDesc->label->Width;
	pDesc->various->Height = pObject->Height;
	pDesc->various->x = pDesc->label->Width;

	PX_ObjectSetEnabled(pDesc->various, pDesc->editable);

}

px_void PX_Object_VariousEditSetMaxLength(PX_Object* pObject, px_int len)
{
	PX_Object_Various* pDesc = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_VARIOUS);
	PX_ASSERTIF(pDesc == PX_NULL);
	PX_Object_EditSetMaxTextLength(pDesc->various, len);

	
}

PX_Object* PX_Object_VariousAttachObject( PX_Object* pObject,px_int attachIndex, px_int x, px_int y, px_int Width, px_int Height, const px_char* Text, PX_OBJECT_VARIOUS_TYPE type, PX_FontModule* fm)
{
	px_memorypool* mp= pObject->mp;
	PX_Object_Various * pdesc;

	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_Various*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_VARIOUS, 0, PX_Object_VariousRender, 0, 0, sizeof(PX_Object_Various));
	PX_ASSERTIF(pdesc == PX_NULL);


	pdesc->label = PX_Object_LabelCreate(mp, pObject, 0, 0, 1, 1, Text, fm, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pdesc->editable = PX_TRUE;
	pdesc->type = type;
	switch (type)
	{
	case PX_OBJECT_VARIOUS_TYPE_EDIT:
	{
		pdesc->various = PX_Object_EditCreate(mp, pObject, 0, 0, 1, 1, fm);
		PX_ObjectRegisterEvent(pdesc->various, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_VariousOnEditChange, pObject);
	}
	break;
	case PX_OBJECT_VARIOUS_TYPE_LABEL:
	{
		pdesc->various = PX_Object_LabelCreate(mp, pObject, 0, 0, 1, 1, "", fm, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	}
	break;
	case PX_OBJECT_VARIOUS_TYPE_SELECTBAR:
	{
		pdesc->various = PX_Object_SelectBarCreate(mp, pObject, 0, 0, 1, 1, fm);
		PX_ObjectRegisterEvent(pdesc->various, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_VariousOnEditChange, pObject);
	}
	break;
	default:
		break;
	}

	return pObject;
}


PX_Object* PX_Object_VariousCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, const px_char* Text, PX_OBJECT_VARIOUS_TYPE type,  PX_FontModule* fm)
{
	PX_Object* pObject;

	pObject=PX_ObjectCreate(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)Width, (px_float)Height, 0);
	if (!pObject) return PX_NULL;
	if (!PX_Object_VariousAttachObject(pObject, 0, x, y, Width, Height, Text, type, fm))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;
}

PX_Object_Various* PX_Object_GetVarious(PX_Object* pObject)
{
	PX_Object_Various* pDesc = PX_ObjectGetDescByType(pObject, PX_OBJECT_TYPE_VARIOUS);
	PX_ASSERTIF(pDesc == PX_NULL);
	return pDesc;
}

px_void PX_Object_VariousSetEditStyle(PX_Object* pObject,PX_OBJECT_VARIOUS_EIDT_TYPE type)
{

	PX_Object_Various* pDesc = PX_Object_GetVarious( pObject);
	if (pDesc->type==PX_OBJECT_VARIOUS_TYPE_EDIT)
	{
		switch (type)
		{
		case PX_OBJECT_VARIOUS_EDIT_TYPE_INT:
		{
			PX_Object_EditSetInputMode(pDesc->various, PX_OBJECT_EDIT_INPUT_MODE_INTEGER);
		}
		break;
		case PX_OBJECT_VARIOUS_EDIT_TYPE_FLOAT:
		{
			PX_Object_EditSetInputMode(pDesc->various, PX_OBJECT_EDIT_INPUT_MODE_FLOAT);
		}
		break;
		case PX_OBJECT_VARIOUS_EDIT_TYPE_HEX:
		{
			PX_Object_EditSetLimit(pDesc->various, "-01234567890ABCDEFabcdef");
			PX_Object_EditSetInputMode(pDesc->various, PX_OBJECT_EDIT_INPUT_MODE_UPPERCASE);
		}
		break;
		default:
			break;
		}
	}
	
}

const px_char* PX_Object_VariousGetText(PX_Object* pObject)
{

	PX_Object_Various* pDesc = PX_Object_GetVarious(pObject);
	switch (pDesc->type)
	{
	case PX_OBJECT_VARIOUS_TYPE_EDIT:
	{
		return PX_Object_EditGetText(PX_Object_GetVarious(pObject)->various);
	}
	case PX_OBJECT_VARIOUS_TYPE_LABEL:
	{
		return PX_Object_LabelGetText(PX_Object_GetVarious(pObject)->various);
	}
	case PX_OBJECT_VARIOUS_TYPE_SELECTBAR:
	{
		return PX_Object_SelectBarGetCurrentText(PX_Object_GetVarious(pObject)->various);
	}
	default:
		break;
	}
	PX_ASSERT();//error
	return PX_NULL;

}

px_void PX_Object_VariousSetText(PX_Object* pObject, const px_char* Text)
{
	PX_Object_Various* pDesc = PX_Object_GetVarious(pObject);
	switch (pDesc->type)
	{
	case PX_OBJECT_VARIOUS_TYPE_EDIT:
	{
		
		PX_Object_EditSetText(PX_Object_GetVarious(pObject)->various,Text);
		PX_Object_EditSetXYOffset(PX_Object_GetVarious(pObject)->various, 0, 0);
	}
	break;
	case PX_OBJECT_VARIOUS_TYPE_LABEL:
	{
		PX_Object_LabelSetText(PX_Object_GetVarious(pObject)->various, Text);
	}
	break;
	case PX_OBJECT_VARIOUS_TYPE_SELECTBAR:
	{
		PX_ASSERT();
		return;
	}
	default:
		break;
	}

}

px_void PX_Object_VariousAddItem(PX_Object* pObject, const px_char* ItemText)
{
	PX_Object_Various* pDesc = PX_Object_GetVarious(pObject);
	switch (pDesc->type)
	{
	case PX_OBJECT_VARIOUS_TYPE_SELECTBAR:
	{
		PX_Object_SelectBarAddItem(pDesc->various, ItemText);
		return;
	}
	default:
		break;
	}

}

const px_char* PX_Object_VariousGetLabelText(PX_Object* pObject)
{
	return PX_Object_LabelGetText(PX_Object_GetVarious(pObject)->label);
}

px_int PX_Object_VariousSelectBarGetCurrentIndex(PX_Object* pObject)
{
	PX_Object_Various* pDesc = PX_Object_GetVarious(pObject);
	if (pDesc->type==PX_OBJECT_VARIOUS_TYPE_SELECTBAR)
	{
		return PX_Object_SelectBarGetCurrentIndex(pDesc->various);
	}
	return 0;
	
}

px_void PX_Object_VariousSetLabelText(PX_Object* pObject, const px_char* Text)
{
	PX_Object_LabelSetText(PX_Object_GetVarious(pObject)->label, Text);
}

px_void PX_Object_VariousSetTextColor(PX_Object* pObject, px_color Color)
{
	PX_Object_EditSetTextColor(PX_Object_GetVarious(pObject)->various, Color);
	PX_Object_LabelSetTextColor(PX_Object_GetVarious(pObject)->label, Color);
}

px_void PX_Object_VariousSetBackgroundColor(PX_Object* pObject, px_color Color)
{
	PX_Object_Various* pDesc = PX_Object_GetVarious(pObject);
	switch (pDesc->type)
	{
	case PX_OBJECT_VARIOUS_TYPE_EDIT:
	{
		PX_Object_LabelSetBackgroundColor(PX_Object_GetVarious(pObject)->label, Color);
		PX_Object_EditSetBackgroundColor(PX_Object_GetVarious(pObject)->various, Color);
	}
	break;
	case PX_OBJECT_VARIOUS_TYPE_LABEL:
	{
		PX_Object_LabelSetBackgroundColor(PX_Object_GetVarious(pObject)->label, Color);
		PX_Object_LabelSetBackgroundColor(PX_Object_GetVarious(pObject)->various, Color);
	}
	break;
	case PX_OBJECT_VARIOUS_TYPE_SELECTBAR:
	{
		PX_Object_LabelSetBackgroundColor(PX_Object_GetVarious(pObject)->label, Color);
		PX_Object_SelectBarSetBackgroundColor(PX_Object_GetVarious(pObject)->various, Color);
	}
	break;
	default:
		break;
	}
	
}

px_void PX_Object_VariousSetAlign(PX_Object* pObject, PX_ALIGN Align)
{
	PX_Object_LabelSetAlign(PX_Object_GetVarious(pObject)->label, Align);
}

px_void PX_Object_VariousSetString(PX_Object* pObject, const px_char* Text)
{

	PX_Object_VariousSetText(pObject, Text);
}

px_void PX_Object_VariousSetFloat(PX_Object* pObject, float f)
{
	px_char content[32];
	PX_ftoa(f, content, sizeof(content), 6);
	PX_Object_VariousSetText(pObject, content);
}

px_void PX_Object_VariousSetInt(PX_Object* pObject, px_int i)
{
	px_char content[32];
	PX_itoa(i, content, sizeof(content),10);
	
	PX_Object_VariousSetText(pObject, content);
}

px_void PX_Object_VariousSetHex(PX_Object* pObject, px_dword i)
{
	px_char content[32];

	PX_itoa(i, content, sizeof(content), 16);

	PX_Object_VariousSetText(pObject, content);
}
px_void PX_Object_VariousSetBool(PX_Object* pObject, px_bool b)
{
	if (b)
	{
		PX_Object_VariousSetText(pObject, "true");
	}
	else
	{
		PX_Object_VariousSetText(pObject, "false");
	}
}

