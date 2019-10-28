#ifndef PX_CONSOLE_H
#define PX_CONSOLE_H


#include "PainterEngine_Runtime.h"

#define PC_CONSOLE_DEFAULT_MAX_COLUMN 48
#define PC_RESOURCE_PATH "."

typedef px_bool (*console_registry_hostcall)(PX_ScriptVM_Instance *Ins);
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
	union
	{
		px_void    *user_ptr;
		px_int      user_int;
		px_float    user_float;
	};
	
	const px_char *script_header_append;
	console_registry_hostcall registry_call;
}PX_Console;

typedef struct
{
	px_int id;
	PX_Object *Object;
}PX_ConsoleColumn;


px_bool		PX_ConsoleInitialize(PX_Runtime *runtime,PX_Console *pc);
px_void		PX_ConsoleRegistryScriptHeader(PX_Console *pc,const px_char *header);
px_void		PX_ConsoleRegistryHostCall(PX_Console *pc,console_registry_hostcall call_func);
px_void		PX_ConsoleShow(PX_Console *pc,px_bool b);
px_void		PX_ConsolePostEvent(PX_Console *pc,PX_Object_Event e);
px_void		PX_ConsoleUpdate(PX_Console *pc,px_dword elpased);
px_void		PX_ConsoleRender(PX_Console *pc,px_dword elpased);
PX_Object * PX_ConsolePrintText(PX_Console *pc,px_char *text);
PX_Object * PX_ConsolePrintImage(PX_Console *pc,px_char *res_image_key);
PX_Object * PX_ConsolePrintShape(PX_Console *pc,px_char *res_image_key,px_color color);
PX_Object * PX_ConsolePrintAnimation(PX_Console *pc,px_char *res_animation_key);
PX_Object * PX_ConsolePrintPartical(PX_Console *pc,px_int x,px_int y,px_char *res_texture,px_char *script,px_char *_init,px_char *_create,px_char *_updata);
PX_Object * PX_ConsoleShowImage(PX_Console *pc,px_char *res_image_key);
px_void     PX_ConsoleClear(PX_Console *pc);
px_bool		PX_ConsoleExecute(PX_Console *pc,char *pshellstr);

#endif

