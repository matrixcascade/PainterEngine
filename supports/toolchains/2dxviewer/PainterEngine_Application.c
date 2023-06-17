#include "PainterEngine_Application.h"
PX_Application App;
extern void PX_RequestData(const char url[], void* buffer, int size, void* ptr, void (*func_callback)(void* buffer, int size, void* ptr));
px_char RequestDataCache[1024 * 1024 * 32];
px_int size=0;


px_void PX_Object_AnimationOnLoad(px_void* buffer, px_int size, px_void* ptr)
{
	PX_Application *pApp=(PX_Application *)ptr;
	PX_Object_ListClear(pApp->list_Animations);
	
	if (pApp->animationLibrary.mp)
	{
		PX_AnimationLibraryFree(&pApp->animationLibrary);
		PX_memset(&pApp->animationLibrary, 0, sizeof(PX_AnimationLibrary));
		PX_AnimationFree(&pApp->animation);
	}
	if (!PX_AnimationLibraryCreateFromMemory(&pApp->runtime.mp_resources, &pApp->animationLibrary, buffer, size))
	{
		PX_Object_MessageBoxAlertOk(pApp->messagebox,"Load Animation Failed", PX_NULL,PX_NULL);
	}
	else
	{
		px_int i;
		for (i = 0; i < pApp->animationLibrary.animation.size; i++)
		{
			PX_Animationlibrary_tagInfo *ptag=PX_VECTORAT(PX_Animationlibrary_tagInfo,&pApp->animationLibrary.animation,i);
			if(ptag->name.buffer[0]!='$'&& ptag->name.buffer[0] != '*' && ptag->name.buffer[0] != '?' && ptag->name.buffer[0] != '@' && ptag->name.buffer[0] != '#' && ptag->name.buffer[0] != '!')
				PX_Object_ListAdd(pApp->list_Animations, ptag->name.buffer);
		}
		PX_AnimationCreate(&pApp->animation, &pApp->animationLibrary);
	}

}

px_void PX_Object_MenuOnOpenFile(px_void* user_ptr)
{
	PX_Application *pApp=(PX_Application *)user_ptr;
	PX_RequestData("open", RequestDataCache, sizeof(RequestDataCache),pApp, PX_Object_AnimationOnLoad);
}

px_void PX_Object_AnimationsListItemOnRender(px_surface* psurface, PX_Object* pObject, px_dword elapsed)
{
	px_char* pdata = PX_Object_ListItemGetData(pObject);
	
	PX_FontDrawText(psurface, (px_int)pObject->x+3, (px_int)(pObject->y+pObject->Height/2), PX_ALIGN_LEFTMID, pdata, PX_OBJECT_UI_DEFAULT_FONTCOLOR);
}

px_bool PX_Object_AnimationsListItemOnCreate(px_memorypool* mp, PX_Object* ItemObject, px_void* userptr)
{
	PX_Object* pObject = (PX_Object*)userptr;
	ItemObject->Func_ObjectRender = PX_Object_AnimationsListItemOnRender;
	ItemObject->User_ptr = userptr;
	return PX_TRUE;
}

px_void PX_Object_AnimationListOnClick(PX_Object* pObject, PX_Object_Event e, px_void* user_ptr)
{
	PX_Application *pApp=(PX_Application *)user_ptr;
	px_int index=PX_Object_ListGetCurrentSelectIndex(pApp->list_Animations);
	if (pApp->animationLibrary.mp==0)
	{
		return;
	}
	if (index != -1)
	{
		px_char *animationName=PX_Object_ListGetItemData(pApp->list_Animations,index);
		PX_AnimationPlay(&pApp->animation, animationName);
	}
}

px_float PX_ApplicationGetAnimationSpeed(PX_Application* pApp)
{
	px_int value= PX_Object_SliderBarGetValue(pApp->sliderbar_speed);
	if (value>0)
	{
		return 1+value / 100.f * 5;
	}
	else
	{
		return (100 + value)/100.f;

	}
}
px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);
	pApp->root=PX_ObjectCreate(&pApp->runtime.mp_ui,PX_NULL,0,0,0,0,0,0);
	pApp->menu=PX_Object_MenuCreate(&pApp->runtime.mp_ui,pApp->root,0,0,256,0);
	PX_Object_MenuAddItem(pApp->menu,0,"Load animation from file", PX_Object_MenuOnOpenFile,pApp);
	pApp->widget = PX_Object_WidgetCreate(&pApp->runtime.mp_ui, pApp->root, pApp->runtime.surface_width-168, 48, 160, 380, "Animations", 0);
	pApp->list_Animations=PX_Object_ListCreate(&pApp->runtime.mp_ui,pApp->widget,0,0,160,320,24, PX_Object_AnimationsListItemOnCreate,pApp);
	PX_ObjectRegisterEvent(pApp->list_Animations,PX_OBJECT_EVENT_EXECUTE,PX_Object_AnimationListOnClick,pApp);

	pApp->sliderbar_speed=PX_Object_SliderBarCreate(&pApp->runtime.mp_ui,pApp->widget,0,325,160,20,PX_OBJECT_SLIDERBAR_TYPE_HORIZONTAL,PX_OBJECT_SLIDERBAR_STYLE_BOX);
	PX_Object_SliderBarSetRange(pApp->sliderbar_speed, -100, 100);
	pApp->messagebox =PX_Object_MessageBoxCreate(&pApp->runtime.mp_ui,pApp->root,0);
	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elapsed)
{
	if (pApp->animationLibrary.mp)
	{
		px_float speed=PX_ApplicationGetAnimationSpeed(pApp);
		if (elapsed * speed<1&& elapsed * speed)
		{
			PX_AnimationUpdate(&pApp->animation, 1);
		}
		else
		PX_AnimationUpdate(&pApp->animation,(px_dword)(elapsed*speed));
	}
}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elapsed)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;

	PX_RuntimeRenderClear(&pApp->runtime,PX_OBJECT_UI_DEFAULT_BACKGROUNDCOLOR);
	if (pApp->animationLibrary.mp)
	{
		px_char content[128];
		PX_AnimationRender(pRenderSurface, &pApp->animation, pRenderSurface->width/2, pRenderSurface->height / 2, PX_ALIGN_CENTER, PX_NULL);
		PX_sprintf8(content,sizeof(content),"frame %1 clip x:%2 y:%3 w:%4 h:%5 i:%6 speed:%7.2 x memory:~%8 bytes",\
		PX_STRINGFORMAT_INT(pApp->animation.reg_currentFrameIndex),\
		PX_STRINGFORMAT_INT(pApp->animation.reg_clipx), \
			PX_STRINGFORMAT_INT(pApp->animation.reg_clipy), \
			PX_STRINGFORMAT_INT(pApp->animation.reg_clipw), \
			PX_STRINGFORMAT_INT(pApp->animation.reg_cliph), \
			PX_STRINGFORMAT_INT(pApp->animation.reg_clipi), \
			PX_STRINGFORMAT_FLOAT(PX_ApplicationGetAnimationSpeed(pApp)), \
			PX_STRINGFORMAT_INT(pApp->runtime.mp_resources.Size- pApp->runtime.mp_resources.FreeSize)\
		);
		PX_FontDrawText(pRenderSurface, pRenderSurface->width / 2, pRenderSurface->height - 24, PX_ALIGN_MIDTOP, content, PX_COLOR_GREEN);
	}

	PX_ObjectRender(pRenderSurface,pApp->root,elapsed);
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ApplicationEventDefault(&pApp->runtime, e);
	PX_ObjectPostEvent(pApp->root, e);
}

