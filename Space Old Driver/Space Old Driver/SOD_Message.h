#ifndef SOD_MESSAGE_H
#define SOD_MESSAGE_H

#include "SOD_Runtime.h"

#define SOD_MESSAGE_STAGE_1_HEIGHT 10
#define SOD_MESSAGE_STAGE_1_TIME 60
#define SOD_MESSAGE_STAGE_2_HEIGHT 200
#define SOD_MESSAGE_STAGE_2_TIME 240

typedef enum
{
	SOD_MESSAGE_MODE_EXPAND,
	SOD_MESSAGE_MODE_CLOSE
}SOD_MESSAGE_MODE;

typedef struct
{
	px_bool show;
	px_int window_Width,window_Height;
	PX_Runtime *runtime;
	px_shape *pShape;
	px_int schedule;
	px_bool ret;
	SOD_MESSAGE_MODE mode;
	PX_Object *root,*btn_Yes,*btn_No;
}SOD_Message;


px_bool SOD_MessageInitialize(PX_Runtime *runtime,SOD_Message *pm,px_int window_Width,px_int window_Height);
px_void SOD_MessagePostEvent(SOD_Message *pm,PX_Object_Event e);
px_void SOD_MessageUpdate(SOD_Message *pm,px_dword elpased);
px_void SOD_MessageRender(SOD_Message *pm,px_dword elpased);
px_void SOD_MessageAlert_OK(SOD_Message *pm,px_char tex_key[]);
px_void SOD_MessageAlert_YESNO(SOD_Message *pm,px_char tex_key[]);
#endif