#ifndef SS_MESSAGEBOX_H
#define SS_MESSAGEBOX_H

#include "PainterEngine_Runtime.h"

#define PX_MESSAGEBOX_DEFAULT_STAGE_1_HEIGHT 10
#define PX_MESSAGEBOX_DEFAULT_STAGE_1_TIME 60
#define PX_MESSAGEBOX_DEFAULT_STAGE_2_HEIGHT 200
#define PX_MESSAGEBOX_DEFAULT_STAGE_2_TIME 240

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

typedef px_void  (*PX_MessageBoxCallBack)(px_void *ptr);

typedef struct
{
	px_bool show;
	px_int window_Width,window_Height;
	PX_Runtime *runtime;
	px_int schedule;
	PX_MESSAGEBOX_RETURN retVal;
	PX_MESSAGEBOX_MODE mode;
	PX_MESSAGEBOX_COLORMOD colormod;

	px_int PX_MESSAGEBOX_STAGE_1_HEIGHT;
	px_int PX_MESSAGEBOX_STAGE_2_HEIGHT;

	PX_MessageBoxCallBack function_yes,function_no;
	px_void *function_yes_ptr,*function_no_ptr;
	const px_char *Message;
	PX_FontModule *fontmodule;
	PX_Object *root,*btn_Ok,*btn_Cancel,*edit_inputbox;
}PX_MessageBox;


px_bool PX_MessageBoxInitialize(PX_Runtime *runtime,PX_MessageBox *pm,PX_FontModule *fontmodule);
px_void PX_MessageBoxPostEvent(PX_MessageBox *pm,PX_Object_Event e);
px_void PX_MessageBoxUpdate(PX_MessageBox *pm,px_dword elpased);
px_void PX_MessageBoxClose(PX_MessageBox *pm);
px_void PX_MessageBoxRender(px_surface *pSurface,PX_MessageBox *pm,px_dword elpased);
px_void PX_MessageBoxAlertOk(PX_MessageBox *pm,const px_char *message);
px_void PX_MessageBoxAlertOkEx(PX_MessageBox *pm,const px_char *message,PX_MessageBoxCallBack func_callback,px_void *ptr);
px_void PX_MessageBoxAlert(PX_MessageBox *pm,const px_char *message);
px_void PX_MessageBoxAlertYesNo(PX_MessageBox *pm,const char *Message);
px_void PX_MessageBoxAlertYesNoEx(PX_MessageBox *pm,const char *Message,PX_MessageBoxCallBack func_yescallback,px_void *yesptr,PX_MessageBoxCallBack func_nocallback,px_void *noptr);
px_void PX_MessageBoxInputBox(PX_MessageBox *pm,const char *Message,PX_MessageBoxCallBack func_yescallback,px_void *yesptr,PX_MessageBoxCallBack func_cancelcallback,px_void *cancelptr);
px_char *PX_MessageBoxGetInput(PX_MessageBox *pm);

px_void PX_MessageBoxSetColorModule(PX_MessageBox *pm,PX_MESSAGEBOX_COLORMOD colormod);
px_void PX_MessageBoxSetFontModule(PX_MessageBox *pm,PX_FontModule *fm);
PX_MESSAGEBOX_RETURN SS_MessageBoxGetLastReturn(PX_MessageBox *pm);
#endif