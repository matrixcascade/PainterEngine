#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H

#ifdef __cplusplus
extern "C"{
#endif

#include "PainterEngine_Startup.h"
	typedef struct
	{
		PX_Object* root;
		PX_Object* menu;
		PX_Object *widget,*list_Animations,*sliderbar_speed,*messagebox;
		PX_AnimationLibrary animationLibrary;
		PX_Animation animation;
		PX_Runtime runtime;
	}PX_Application;

	extern PX_Application App;

	px_bool PX_ApplicationInitialize(PX_Application* App, px_int screen_Width, px_int screen_Height);
	px_void PX_ApplicationUpdate(PX_Application* App, px_dword elapsed);
	px_void PX_ApplicationRender(PX_Application* App, px_dword elapsed);
	px_void PX_ApplicationPostEvent(PX_Application* App, PX_Object_Event e);
#ifdef __cplusplus
}
#endif

#endif
