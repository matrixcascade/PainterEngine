#ifndef SS_MESSAGEBOX_H
#define SS_MESSAGEBOX_H

#include "PainterEngine_Runtime.h"

#define PX_MESSAGEBOX_BOX_STAGE_1_HEIGHT 10
#define PX_MESSAGEBOX_STAGE_1_TIME 60
#define PX_MESSAGEBOX_STAGE_2_HEIGHT 200
#define PX_MESSAGEBOX_STAGE_2_TIME 240

typedef enum
{
	PX_MESSAGEBOX_MODE_EXPAND,
	PX_MESSAGEBOX_MODE_CLOSE
}PX_MESSAGEBOX_MODE;


typedef enum
{
	PX_MESSAGEBOX_RETURN_NONE,
	PX_MESSAGEBOX_RETURN_YES,
	PX_MESSAGEBOX_RETURN_NO
}PX_MESSAGEBOX_RETURN;

typedef enum
{
	PX_MESSAGEBOX_COLORMOD_LIGHT,
	PX_MESSAGEBOX_COLORMOD_NIGHT,
}PX_MESSAGEBOX_COLORMOD;

typedef struct
{
	px_bool show;
	px_int window_Width,window_Height;
	PX_Runtime *runtime;
	px_int schedule;
	PX_MESSAGEBOX_RETURN retVal;
	PX_MESSAGEBOX_MODE mode;
	PX_MESSAGEBOX_COLORMOD colormod;
	const px_char *Message;
	PX_FontModule *fontmodule;
	PX_Object *root,*btn_Ok,*btn_Cancel;
}PX_MessageBox;


px_bool PX_MessageBoxInitialize(PX_Runtime *runtime,PX_MessageBox *pm,PX_FontModule *fontmodule,px_int window_Width,px_int window_Height);
px_void PX_MessageBoxPostEvent(PX_MessageBox *pm,PX_Object_Event e);
px_void PX_MessageBoxUpdate(PX_MessageBox *pm,px_dword elpased);
px_void PX_MessageBoxClose(PX_MessageBox *pm);
px_void PX_MessageBoxRender(px_surface *pSurface,PX_MessageBox *pm,px_dword elpased);
px_void PX_MessageBoxAlertOk(PX_MessageBox *pm,const px_char *message);
px_void PX_MessageBoxYesNo(PX_MessageBox *pm,const char *Message);
px_void PX_MessageBoxSetColorModule(PX_MessageBox *pm,PX_MESSAGEBOX_COLORMOD colormod);
PX_MESSAGEBOX_RETURN SS_MessageBoxGetLastReturn(PX_MessageBox *pm);
#endif