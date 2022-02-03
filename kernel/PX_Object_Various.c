#include "PX_Object_Various.h"

px_void PX_Object_VariousOnEditChange(PX_Object* pEObject, PX_Object_Event e, px_void* ptr)
{
	PX_Object* pObject = (PX_Object*)ptr;
	PX_ObjectExecuteEvent(pObject, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_VALUECHANGED));
}


px_void PX_Object_VariousRender(px_surface *psurface,PX_Object* pObject,px_dword elapsed)
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
	PX_Object_Various* pDesc = PX_ObjectGetDesc(PX_Object_Various, pObject);
	if (pDesc->type==PX_OBJECT_VARIOUS_TYPE_EDIT)
	{
		PX_Object_EditSetMaxTextLength(pDesc->various, len);
	}
	
}

PX_Object* PX_Object_VariousCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int Width, px_int Height, const px_char* Text, PX_OBJECT_VARIOUS_TYPE type,  PX_FontModule* fm)
{
	PX_Object_Various var,*pdesc;
	PX_Object* pObject;
	PX_zeromemory(&var,sizeof(var));
	pObject=PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)Width, (px_float)Height, 0,PX_OBJECT_TYPE_VARIOUS,PX_NULL, PX_Object_VariousRender,PX_NULL,&var,sizeof(var));
	pdesc = PX_ObjectGetDesc(PX_Object_Various, pObject);
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
		pdesc->various = PX_Object_LabelCreate(mp, pObject, 0, 0, 1, 1,"",fm, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	}
	break;
	case PX_OBJECT_VARIOUS_TYPE_SELECTBAR:
	{
		pdesc->various = PX_Object_SelectBarCreate(mp, pObject, 0, 0, 1, 1,fm);
		PX_ObjectRegisterEvent(pdesc->various, PX_OBJECT_EVENT_VALUECHANGED, PX_Object_VariousOnEditChange, pObject);
	}
	break;
	default:
		break;
	} 
	
	return pObject;
}

PX_Object_Various* PX_Object_GetVarious(PX_Object* pObject)
{
	if (pObject->Type== PX_OBJECT_TYPE_VARIOUS)
	{
		return  PX_ObjectGetDesc(PX_Object_Various, pObject);
	}
	return PX_NULL;
}

px_void PX_Object_VariousSetEditStyle(PX_Object* pObject,PX_OBJECT_VARIOUS_EIDT_TYPE type)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	else
	{
		PX_Object_Various* pDesc = PX_ObjectGetDesc(PX_Object_Various, pObject);
		if (pDesc->type==PX_OBJECT_VARIOUS_TYPE_EDIT)
		{
			switch (type)
			{
			case PX_OBJECT_VARIOUS_EDIT_TYPE_INT:
			{
				PX_Object_EditSetLimit(pDesc->various, "-01234567890");
			}
			break;
			case PX_OBJECT_VARIOUS_EDIT_TYPE_FLOAT:
			{
				PX_Object_EditSetLimit(pDesc->various, "-01234567890.");
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
}

const px_char* PX_Object_VariousGetText(PX_Object* pObject)
{
	if (pObject->Type!= PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	else
	{
		PX_Object_Various* pDesc = PX_ObjectGetDesc(PX_Object_Various, pObject);
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
	}

	PX_ASSERT();
	return PX_NULL;
	
}

px_void PX_Object_VariousSetText(PX_Object* pObject, const px_char* Text)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	else
	{
		PX_Object_Various* pDesc = PX_ObjectGetDesc(PX_Object_Various, pObject);
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

}

px_void PX_Object_VariousAddItem(PX_Object* pObject, const px_char* ItemText)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	else
	{
		PX_Object_Various* pDesc = PX_ObjectGetDesc(PX_Object_Various, pObject);
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
	PX_ASSERT();
	return;
}

const px_char* PX_Object_VariousGetLabelText(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	return PX_Object_LabelGetText(PX_Object_GetVarious(pObject)->label);
}

px_int PX_Object_VariousSelectBarGetCurrentIndex(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	else
	{
		PX_Object_Various* pDesc = PX_ObjectGetDesc(PX_Object_Various, pObject);
		if (pDesc->type==PX_OBJECT_VARIOUS_TYPE_SELECTBAR)
		{
			return PX_Object_SelectBarGetCurrentIndex(pDesc->various);
		}
	}
	return 0;
}

px_void PX_Object_VariousSetLabelText(PX_Object* pObject, const px_char* Text)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	PX_Object_LabelSetText(PX_Object_GetVarious(pObject)->label, Text);
}

px_void PX_Object_VariousSetTextColor(PX_Object* pObject, px_color Color)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	PX_Object_EditSetTextColor(PX_Object_GetVarious(pObject)->various, Color);
	PX_Object_LabelSetTextColor(PX_Object_GetVarious(pObject)->label, Color);
}

px_void PX_Object_VariousSetBackgroundColor(PX_Object* pObject, px_color Color)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	else
	{
		PX_Object_Various* pDesc = PX_ObjectGetDesc(PX_Object_Various, pObject);
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
}

px_void PX_Object_VariousSetAlign(PX_Object* pObject, PX_ALIGN Align)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	PX_Object_LabelSetAlign(PX_Object_GetVarious(pObject)->label, Align);
}

px_void PX_Object_VariousSetString(PX_Object* pObject, const px_char* Text)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	PX_Object_VariousSetText(pObject, Text);
}

px_void PX_Object_VariousSetFloat(PX_Object* pObject, float f)
{
	px_char content[32];
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	PX_ftoa(f, content, sizeof(content), 6);
	PX_Object_VariousSetText(pObject, content);
}

px_void PX_Object_VariousSetInt(PX_Object* pObject, int i)
{
	px_char content[32];
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	PX_itoa(i, content, sizeof(content),10);
	
	PX_Object_VariousSetText(pObject, content);
}

px_void PX_Object_VariousSetHex(PX_Object* pObject, px_dword i)
{
	px_char content[32];
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	PX_itoa(i, content, sizeof(content), 16);

	PX_Object_VariousSetText(pObject, content);
}
px_void PX_Object_VariousSetBool(PX_Object* pObject, px_bool b)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	if (b)
	{
		PX_Object_VariousSetText(pObject, "true");
	}
	else
	{
		PX_Object_VariousSetText(pObject, "false");
	}
}

