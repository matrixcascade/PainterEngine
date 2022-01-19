#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H

#ifdef __cplusplus
extern "C"{
#endif

#include "PainterEngine_Startup.h"
#ifdef __cplusplus
}
#endif
typedef struct
{
	PX_Object *ui_root;
	PX_Object *button;	//按钮
	PX_Object * edit;  	//文本输入框
	px_texture MyTexture; 	//图片纹理
    PX_Object *designer;    //设计器

	PX_FontModule fm;//中文字模
	PX_Runtime runtime;
}PX_Application;

extern PX_Application App;

px_bool PX_ApplicationInitialize(PX_Application *App,px_int screen_Width,px_int screen_Height);
px_void PX_ApplicationUpdate(PX_Application *App,px_dword elapsed);
px_void PX_ApplicationRender(PX_Application *App,px_dword elapsed);
px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e);

#endif
