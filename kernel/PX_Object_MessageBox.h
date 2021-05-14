#ifndef PX_OBJECT_MESSAGEBOX_H
#define PX_OBJECT_MESSAGEBOX_H
#include "PX_Object.h"


#define PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_1_HEIGHT 10
#define PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_1_TIME 60
#define PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_2_HEIGHT 200
#define PX_OBJECT_MESSAGEBOX_DEFAULT_STAGE_2_TIME 240
#define PX_OBJECT_MESSAGEBOX_MAX_MESSAGESIZE 128

typedef enum
{
	PX_OBJECT_MESSAGEBOX_MODE_EXPAND,
	PX_OBJECT_MESSAGEBOX_MODE_CLOSE
}PX_OBJECT_MESSAGEBOX_MODE;


typedef enum
{
	PX_MESSAGEBOX_COLORMOD_LIGHT,
	PX_MESSAGEBOX_COLORMOD_NIGHT,
}PX_MESSAGEBOX_COLORMOD;

struct _PX_Object_MessageBox;

typedef px_void  (*PX_Object_MessageBoxCallBack)(PX_Object *pmessagebox,PX_Object_Event e, px_void *ptr);

typedef struct _PX_Object_MessageBox
{
	px_int schedule;

	PX_OBJECT_MESSAGEBOX_MODE mode;
	PX_MESSAGEBOX_COLORMOD colormod;

	px_int PX_MESSAGEBOX_STAGE_1_HEIGHT;
	px_int PX_MESSAGEBOX_STAGE_2_HEIGHT;

	PX_Object_MessageBoxCallBack function_yes,function_no;
	px_void *function_yes_ptr,*function_no_ptr;
	px_char Message[PX_OBJECT_MESSAGEBOX_MAX_MESSAGESIZE];
	PX_FontModule *fontmodule;
	PX_Object *btn_Ok,*btn_Cancel,*edit_inputbox;

	px_color fillbackgroundcolor;
}PX_Object_MessageBox;


PX_Object_MessageBox *PX_Object_GetMessageBox(PX_Object *pObject);
px_void PX_Object_MessageBoxClose(PX_Object *pObject);
px_void PX_Object_MessageBoxAlertOk(PX_Object *pObject,const px_char *message,PX_Object_MessageBoxCallBack func_callback,px_void *ptr);
px_void PX_Object_MessageBoxAlert(PX_Object *pObject,const px_char *message);
px_void PX_Object_MessageBoxAlertYesNo(PX_Object *pObject,const char *Message,PX_Object_MessageBoxCallBack func_yescallback,px_void *yesptr,PX_Object_MessageBoxCallBack func_nocallback,px_void *noptr);
px_void PX_Object_MessageBoxInputBox(PX_Object *pObject,const char *Message,PX_Object_MessageBoxCallBack func_yescallback,px_void *yesptr,PX_Object_MessageBoxCallBack func_cancelcallback,px_void *cancelptr);
px_char * PX_Object_MessageBoxGetInput(PX_Object *pObject);
PX_Object * PX_Object_MessageBoxCreate(px_memorypool *mp,PX_Object *parent,PX_FontModule *fontmodule);
#endif

