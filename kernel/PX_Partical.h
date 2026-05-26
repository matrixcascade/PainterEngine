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
	px_void*   user;
}PX_Partical_Atom;

struct _PX_Partical_Launcher;

typedef px_void (* PX_ParticalLauncher_CreateAtom)(struct _PX_Partical_Launcher *launcher, PX_Partical_Atom* pAtom,px_int index);
typedef px_void (* PX_ParticalLauncher_UpdateAtom)(struct _PX_Partical_Launcher *launcher,PX_Partical_Atom *pAtom);

#define PX_PARITCAL_CREATE_FUNCTION(name) px_void name(struct _PX_Partical_Launcher *launcher,PX_Partical_Atom *pAtom,px_int index)
#define PX_PARITCAL_UPDATE_FUNCTION(name) px_void name(struct _PX_Partical_Launcher *launcher,PX_Partical_Atom *pAtom)

typedef struct
{
	px_void			*userptr;//User-defined pointer passed to callbacks for custom context data
	px_texture		*tex;//Source texture used to render each particle atom
	px_point        position;//Base emission position (world coordinates) of the launcher
	px_float		deviation_position_distanceRange;//Maximum random offset distance from base position when spawning a particle

	px_point        toward;//Base emission direction vector for newly spawned particles
	px_float        direction_deviation_rangAngle_max;//Upper bound (in radians) of random angular deviation added to base direction
	px_float        direction_deviation_rangAngle_min;//Lower bound (in radians) of random angular deviation added to base direction

	px_float        velocity;//Base initial speed (magnitude) of newly spawned particles
	px_float        velocity_deviation_max;//Upper bound of random additive deviation applied to base velocity
	px_float        velocity_deviation_min;//Lower bound of random additive deviation applied to base velocity

	px_float        size;//Base initial size (scale factor) of each particle atom
	px_float		size_deviation_max;//Upper bound of random additive deviation applied to base atom size
	px_float		size_deviation_min;//Lower bound of random additive deviation applied to base atom size
	px_float		size_increase;//Per-second growth rate of particle size during its lifetime

	px_float		rotation;//Base initial rotation angle (in radians) of newly spawned particles
	px_float		rotation_deviation;//Maximum random deviation (in radians) applied to base rotation angle

	px_float        alpha;//Base initial alpha (opacity, 0.0 to 1.0) of newly spawned particles
	px_float		alpha_deviation;//Maximum random deviation applied to base alpha value

	px_float        hdrR;//Base HDR red color multiplier applied to particle texture
	px_float		hdrR_deviation;//Maximum random deviation applied to HDR red multiplier

	px_float        hdrG;//Base HDR green color multiplier applied to particle texture
	px_float		hdrG_deviation;//Maximum random deviation applied to HDR green multiplier

	px_float        hdrB;//Base HDR blue color multiplier applied to particle texture
	px_float		hdrB_deviation;//Maximum random deviation applied to HDR blue multiplier

	px_float		alpha_increase;//Per-second change rate of particle alpha during its lifetime (negative for fade-out)

	px_point		a;//Constant acceleration vector applied to each particle every frame (e.g. gravity)
	px_float		ak;//Linear damping coefficient applied to particle velocity (drag/friction factor) 

	px_int			alive;//Lifetime of each particle in milliseconds before it is removed
	px_int			generateDuration;//Time interval in milliseconds between consecutive particle spawns
	px_int			maxCount;//Maximum number of particles allowed alive simultaneously (pool capacity)
	px_int          per_launch_count;//Number of particles to spawn each time the generator emits new particles; if 0, the generator will emit as many particles as possible up to maxCount
	px_int			generate_remain_count;//max generate partical count,when generate_count is greater than 0, the launcher will only generate generate_count particles in total; if -1, there is no limit on total particles generated

	PX_ParticalLauncher_CreateAtom Create_func;//Optional callback to customize particle creation; if NULL the default generator is used
	PX_ParticalLauncher_UpdateAtom Update_func;//Optional callback invoked each frame to customize per-particle update behavior
}PX_ParticalLauncher_InitializeInfo;

typedef struct _PX_Partical_Launcher
{
	px_memorypool   *mp;

	px_bool			close;
	px_dword        elapsed;
	px_int          genIndex;
	px_int			lastgenIndex;
	PX_Partical_Atom *ParticalPool;
	px_float        lefttopX,leftTopY,rightBottomX,rightBottomY;
	PX_ParticalLauncher_InitializeInfo InitInfo,LauncherInfo;
}PX_ParticalLauncher;


px_void PX_ParticalLauncherInitializeDefaultInfo(PX_ParticalLauncher_InitializeInfo* info);
px_bool PX_ParticalLauncherInitialize(PX_ParticalLauncher *launcher,px_memorypool   *mp,PX_ParticalLauncher_InitializeInfo Info);
px_void PX_ParticalLauncherSetPosition(PX_ParticalLauncher *launcher,px_float x,px_float y,px_float z);
px_void PX_ParticalLauncherSetDirection(PX_ParticalLauncher *launcher,px_point toward);
px_void PX_ParticalLauncherOpen(PX_ParticalLauncher* launcher);
px_void PX_ParticalLauncherClose(PX_ParticalLauncher* launcher);

px_void PX_ParticalLauncherReset(PX_ParticalLauncher* launcher);
px_void PX_ParticalLauncherRender(px_surface *surface,PX_ParticalLauncher *launcher,px_dword elapsed);
px_void PX_ParticalLauncherFree(PX_ParticalLauncher *launcher);
#endif
