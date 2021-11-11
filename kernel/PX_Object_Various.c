#include "PX_Object_Various.h"

px_void PX_Object_VariousRender(px_surface *psurface,PX_Object* pObject,px_dword elpased)
{
	PX_Object_Various* pDesc = PX_ObjectGetDesc(PX_Object_Various, pObject);

	pDesc->label->Width = pObject->Width * 0.25f;
	pDesc->label->Height = pObject->Height;

	pDesc->edit->Width = pObject->Width * 0.75f;
	pDesc->edit->Height = pObject->Height;
	pDesc->edit->x = pObject->Width * 0.25f;

	PX_ObjectSetEnabled(pDesc->edit, pDesc->editable);

}

PX_Object* PX_Object_VariousCreate(px_memorypool* mp, PX_Object* Parent, PX_OBJECT_VARIOUS_TYPE type, px_int x, px_int y, px_int Width, px_int Height, const px_char* Text, PX_FontModule* fm)
{
	PX_Object_Various var,*pdesc;
	PX_Object* pObject;
	PX_zeromemory(&var,sizeof(var));
	pObject=PX_ObjectCreateEx(mp, Parent, (px_float)x, (px_float)y, 0, (px_float)Width, (px_float)Height, 0,PX_OBJECT_TYPE_VARIOUS,PX_NULL, PX_Object_VariousRender,PX_NULL,&var,sizeof(var));
	pdesc = PX_ObjectGetDesc(PX_Object_Various, pObject);
	pdesc->edit = PX_Object_EditCreate(mp, pObject, 0, 0, 1, 1, fm);
	pdesc->label = PX_Object_LabelCreate(mp, pObject, 0, 0, 1, 1, Text,fm,PX_OBJECT_UI_DEFAULT_FONTCOLOR);
	pdesc->editable = PX_TRUE;
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

px_char* PX_Object_VariousGetText(PX_Object* pObject)
{
	if (pObject->Type!= PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	return PX_Object_EditGetText(PX_Object_GetVarious(pObject)->edit);
}

px_void PX_Object_VariousSetText(PX_Object* pObject, const px_char* Text)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	PX_Object_EditSetText(PX_Object_GetVarious(pObject)->edit,Text);
}


px_char* PX_Object_VariousGetLabelText(PX_Object* pObject)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	return PX_Object_LabelGetText(PX_Object_GetVarious(pObject)->label);
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
	PX_Object_EditSetTextColor(PX_Object_GetVarious(pObject)->edit, Color);
	PX_Object_LabelSetTextColor(PX_Object_GetVarious(pObject)->label, Color);
}

px_void PX_Object_VariousSetBackgroundColor(PX_Object* pObject, px_color Color)
{
	if (pObject->Type != PX_OBJECT_TYPE_VARIOUS)
	{
		PX_ASSERT();
		return;
	}
	PX_Object_EditSetBackgroundColor(PX_Object_GetVarious(pObject)->edit, Color);
	PX_Object_LabelSetBackgroundColor(PX_Object_GetVarious(pObject)->label, Color);
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
	PX_Object_EditSetText(PX_Object_GetVarious(pObject)->edit, Text);
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
	PX_Object_EditSetText(PX_Object_GetVarious(pObject)->edit, content);
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
	PX_Object_EditSetText(PX_Object_GetVarious(pObject)->edit, content);
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
		PX_Object_EditSetText(PX_Object_GetVarious(pObject)->edit,"TRUE");
	}
	else
	{
		PX_Object_EditSetText(PX_Object_GetVarious(pObject)->edit, "FALSE");
	}
	
}

