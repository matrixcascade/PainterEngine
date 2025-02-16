#include "PX_Object_Collapse.h"

PX_OBJECT_EVENT_FUNCTION(PX_Object_CollapseOnCollapse)
{
	PX_Object_Collapse* pDesc = (PX_Object_Collapse*)PX_ObjectGetDescByType((PX_Object *)ptr,PX_OBJECT_TYPE_COLLAPSE);
	if (pDesc->scrollarea->Visible)
	{
		PX_ObjectSetVisible(pDesc->scrollarea, PX_FALSE);
		PX_Object_PushButtonSetText(pDesc->collapse, "+");
	}
	else
	{
		PX_ObjectSetVisible(pDesc->scrollarea, PX_TRUE);
		PX_Object_PushButtonSetText(pDesc->collapse, "-");
	}
	PX_ObjectExecuteEvent(pDesc->scrollarea, PX_OBJECT_BUILD_EVENT(PX_OBJECT_EVENT_UPDATE));
}
PX_Object* PX_Object_CollapseAttachObject(PX_Object* pObject, px_int attachIndex, px_int x, px_int y, px_int width, px_int height, const px_char* title, PX_FontModule* fontModule)
{
	px_memorypool* mp = pObject->mp;
	PX_Object_Collapse* pdesc;
	PX_ASSERTIF(pObject == PX_NULL);
	PX_ASSERTIF(attachIndex < 0 || attachIndex >= PX_COUNTOF(pObject->pObjectDesc));
	PX_ASSERTIF(pObject->pObjectDesc[attachIndex] != PX_NULL);
	pdesc = (PX_Object_Collapse*)PX_ObjectCreateDesc(pObject, attachIndex, PX_OBJECT_TYPE_COLORPANEL, 0, 0, 0, 0, sizeof(PX_Object_Collapse));
	PX_ASSERTIF(pdesc == PX_NULL);

	pdesc->title = PX_Object_LabelCreate(mp, pObject, 15, 0, width, 28, title, fontModule, PX_COLOR_BLACK);
	if (!pdesc->title)
	{
		PX_LOG("Create registers failed!");
		return PX_NULL;
	}
	PX_Object_LabelSetBorder(pdesc->title, PX_TRUE);
	pdesc->collapse = PX_Object_PushButtonCreate(mp, pdesc->title, (px_int)(pdesc->title->x + pdesc->title->Width - 27), (px_int)pdesc->title->y, 28, 28, "-", 0);
	PX_ObjectRegisterEvent(pdesc->collapse, PX_OBJECT_EVENT_EXECUTE, PX_Object_CollapseOnCollapse, pObject);

	//////////////////////////////////////////////////////////////////////////
	//registers area
	//////////////////////////////////////////////////////////////////////////
	pdesc->scrollarea = PX_Object_ScrollAreaCreate(mp, pObject, \
		0, \
		(px_int)(pdesc->title->y + pdesc->title->Height), \
		width, \
		height);
	return pObject;
}


PX_Object* PX_Object_CollapseCreate(px_memorypool* mp, PX_Object* Parent, px_int x, px_int y, px_int width, px_int height, const px_char* title, PX_FontModule* fontModule)
{
	PX_Object* pObject;

	pObject = PX_ObjectCreate(mp, Parent, x * 1.f, y * 1.f, 0, width * 1.f, height * 1.f, 0);
	if (!pObject)
	{
		return PX_NULL;
	}
	
	if (!PX_Object_CollapseAttachObject(pObject, 0, x, y, width, height,title,fontModule))
	{
		PX_ObjectDelete(pObject);
		return PX_NULL;
	}
	return pObject;

}
