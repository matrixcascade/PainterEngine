#ifndef PX_EXECUTER_H
#define PX_EXECUTER_H

#include "PainterEngine_Runtime.h"

#define PX_EXECUTER_DEFAULT_MAX_COLUMN 48
#define PX_EXECUTER_INPUT_CONTENT_SIZE 1024
#define PX_EXECUTER_THREAD_FRAMEINSTR 1024
typedef struct
{
	PX_Object *Root;
	PX_Object *Area;
	PX_Object *Input;
	px_int max_column;
	px_int column;
	px_bool show;
	px_vector pObjects;
	px_int id;
	PX_Runtime *runtime;
	px_char Input_Content[PX_EXECUTER_INPUT_CONTENT_SIZE];
	px_dword *Sleep;
	px_bool bInput;
	PX_ScriptVM_Instance VM_Instance;
}PX_Executer;

typedef struct
{
	px_int id;
	PX_Object *Object;
}PX_ExecuterColumn;


px_bool		PX_ExecuterInitialize(PX_Runtime *runtime,PX_Executer *pExecuter);
px_void		PX_ExecuterShow(PX_Executer *pExecuter,px_bool bShow);
px_void		PX_ExecuterPostEvent(PX_Executer *pExecuter,PX_Object_Event e);
px_void		PX_ExecuterUpdate(PX_Executer *pExecuter,px_dword elapsed);
px_void		PX_ExecuterRender(PX_Executer *pExecuter,px_dword elapsed);
PX_Object * PX_ExecuterPrintText(PX_Executer *pExecuter,const px_char *text);
PX_Object * PX_ExecuterPrintImage(PX_Executer *pExecuter,const px_char *res_image_key);
px_void     PX_ExecuterClear(PX_Executer *pExecuter);
px_bool		PX_ExecuterRunScipt(PX_Executer *pExecuter,const px_char *pshellstr);
px_bool		PX_ExecuterLoadScipt(PX_Executer* pExecuter, const px_char* pshellstr);
px_bool     PX_ExecuterRegistryHostFunction(PX_Executer *pExecuter,const px_char Name[],PX_ScriptVM_Function_Modules function,px_void *userptr);

#endif

