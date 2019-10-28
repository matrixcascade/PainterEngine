#ifndef PX_PARTICAL_H
#define PX_PARTICAL_H
#include "PX_Script_VM.h"

#define PX_PARTICAL_ATOM_TIME 8 //8ms for partical atom
#define PX_PARTICAL_ONETIME_GENMAX 65535
#define PX_PARTICAL_TRANSFORM_CACHE (8*1024*1024+16)//8M for transform


typedef struct 
{
	px_float   size;
	px_float   rotation;
	px_float   mass;
	px_float   alpha;
	px_dword   aliveTime;
	px_dword   elpasedTime;
	px_point   position;
	px_point   velocity;
	px_float   hdrR,hdrG,hdrB;
	float      roatationSpeed;
	float      sizeIncrement;
	float      alphaIncrement; 
}PX_Partical_Atom;
struct _PX_Partical_Launcher;
typedef PX_Partical_Atom (* PX_ParticalLauncher_CreateAtom)(struct _PX_Partical_Launcher *launcher,px_int index);
typedef px_void (* PX_ParticalLauncher_UpdateAtom)(struct _PX_Partical_Launcher *launcher,PX_Partical_Atom *pAtom);

typedef struct _PX_Partical_Launcher
{
	px_point        launcherPosition;
	px_point        launcherVelocity;

	px_dword		generateDuration;
	px_dword        elpased;
	px_uint			maxCount;
	px_int			launchCount;
	px_uint         genIndex;
	px_point        force;
	px_float        resistanceK;
	px_void			*user;
	px_point        velocity;
	px_point        direction;
	px_uint         CreateParticalFuncIndex;
	px_uint         UpdateParitcalFuncIndex;
	px_float        lefttopX,leftTopY,rightBottomX,rightBottomY;

	PX_ParticalLauncher_CreateAtom Create_func;
	PX_ParticalLauncher_UpdateAtom Update_func;

	px_memorypool   *mp;
	px_texture      *texture;
	PX_Partical_Atom *ParticalPool;
	PX_ScriptVM_Instance *VM_Instance;
}PX_Partical_Launcher;

typedef struct
{
	px_texture *tex;
	px_int		generateDuration;
	px_int		maxCount;
	px_point	force;
	px_float   resistanceK;
	px_int     launchCount;
	PX_ParticalLauncher_CreateAtom Create_func;
	PX_ParticalLauncher_UpdateAtom Update_func;
}PX_ParticalLauncher_InitializeInfo;

px_bool PX_ParticalLauncherCreateEx(PX_Partical_Launcher *launcher,px_memorypool   *mp,PX_ParticalLauncher_InitializeInfo Info);
px_bool PX_ParticalLauncherCreate(PX_Partical_Launcher *launcher,px_memorypool   *mp,px_texture *tex,PX_ScriptVM_Instance *pIns,px_char *Initfunc,px_char *_createfunc,px_char *_updatefunc);
px_void PX_ParticalLauncherSetLauncherPosition(PX_Partical_Launcher *launcher,px_point position);
px_bool PX_ParticalLauncherSetCreateFuncIndex(PX_Partical_Launcher *launcher,px_char *func_Name);
px_bool PX_ParticalLauncherSetUpdateFuncIndex(PX_Partical_Launcher *launcher,px_char *func_Name);

px_bool PX_ParticalLauncherUpdate(PX_Partical_Launcher *launcher,px_dword elpased);
px_void PX_ParticalLauncherRender(px_surface *surface,PX_Partical_Launcher *launcher,px_point position);
px_void PX_ParticalLauncherFree(PX_Partical_Launcher *launcher);
#endif
