#ifndef PX_PARTICAL_H
#define PX_PARTICAL_H
#include "../core/PX_Core.h"

#define PX_PARTICAL_ATOM_TIME 16

typedef struct 
{
	px_texture *ptexture;
	//
	px_point   position;
	px_point   velocity;
	px_dword   elapsedTime;

	//
	px_dword   reg_alive;
	px_float   reg_size;
	px_float   reg_currentrotation;
	px_float   reg_alpha;

	px_float   hdrR, hdrG, hdrB;
	//
	px_float   reg_rotation;
	px_float   reg_sizeinc;
	px_float   reg_alphainc; 
	px_point   reg_a;
	px_float   reg_ak;
}PX_Partical_Atom;

struct _PX_Partical_Launcher;

typedef PX_Partical_Atom (* PX_ParticalLauncher_CreateAtom)(struct _PX_Partical_Launcher *launcher,px_int index);
typedef px_void (* PX_ParticalLauncher_UpdateAtom)(struct _PX_Partical_Launcher *launcher,PX_Partical_Atom *pAtom);


typedef struct
{
	px_void			*userptr;
	px_texture		*tex;
	px_point        position;

	px_point        direction;
	px_float        deviation_rangAngle;
	
	px_float        velocity;
	px_float        deviation_velocity_max;
	px_float        deviation_velocity_min;

	px_float        atomsize;
	px_float		deviation_atomsize_max;
	px_float		deviation_atomsize_min;

	px_float		rotation;
	px_float		deviation_rotation;

	px_float        alpha;
	px_float		deviation_alpha;

	px_float        hdrR;
	px_float		deviation_hdrR;

	px_float        hdrG;
	px_float		deviation_hdrG;

	px_float        hdrB;
	px_float		deviation_hdrB;

	px_float		sizeincrease;
	px_float		alphaincrease;

	px_point		a;
	px_float		ak;

	px_int			alive;
	px_int			generateDuration;
	px_int			maxCount;
	px_int			launchCount;

	PX_ParticalLauncher_CreateAtom Create_func;
	PX_ParticalLauncher_UpdateAtom Update_func;
}PX_ParticalLauncher_InitializeInfo;

typedef struct _PX_Partical_Launcher
{
	px_memorypool   *mp;
	PX_ParticalLauncher_InitializeInfo LauncherInfo;

	px_dword        elapsed;
	px_int          genIndex;
	px_int			lastgenIndex;
	PX_Partical_Atom *ParticalPool;
	px_float        lefttopX,leftTopY,rightBottomX,rightBottomY;
}PX_ParticalLauncher;


px_void PX_ParticalLauncherInitializeDefaultInfo(PX_ParticalLauncher_InitializeInfo* info);
px_bool PX_ParticalLauncherInitialize(PX_ParticalLauncher *launcher,px_memorypool   *mp,PX_ParticalLauncher_InitializeInfo Info);
px_void PX_ParticalLauncherSetPosition(PX_ParticalLauncher *launcher,px_float x,px_float y,px_float z);
px_void PX_ParticalLauncherSetDirection(PX_ParticalLauncher *launcher,px_point direction);
//px_void PX_ParticalLauncherUpdate(PX_ParticalLauncher *launcher,px_dword elapsed);
px_void PX_ParticalLauncherRender(px_surface *surface,PX_ParticalLauncher *launcher,px_dword elapsed);
px_void PX_ParticalLauncherFree(PX_ParticalLauncher *launcher);
#endif
