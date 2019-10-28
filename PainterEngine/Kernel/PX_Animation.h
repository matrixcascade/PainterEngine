#ifndef PX_ANIMATION_H
#define PX_ANIMATION_H
#include "PX_2dxCommon.h"
typedef struct
{
	px_vector frames; //vector frames
	px_vector animation;
	px_memory code;
}px_animationlibrary;

typedef struct
{
	px_uint   elpased;
	px_uint   ip;
	px_uint   reg_loopTimes;
	px_uint   reg_reservedTime;
	px_int    reg_currentFrameIndex;
	px_float  scale;
	px_point  direction;
	px_animationlibrary *linker;
}px_animation;

typedef struct
{
	px_dword ip;
	px_string name; //image file path
}PX_Animationlibrary_tagInfo;

px_bool PX_AnimationCreate(px_animation *animation,px_animationlibrary *linker);
px_bool PX_AnimationSetLibrary(px_animation *animation,px_animationlibrary *linker);
px_void PX_AnimationSetDirection(px_animation *animation,px_point direction);
px_void PX_AnimationLibraryAddInstr(px_animationlibrary *panimationLib,PX_2DX_OPCODE opcode,px_word param);
px_void PX_AnimationLibraryFree(px_animationlibrary *panimationLib);
px_int PX_AnimationGetAnimationsCount(px_animation *animation);
px_bool PX_AnimationSetCurrentPlayAnimation(px_animation *animation,px_int i);
px_bool PX_AnimationSetCurrentPlayAnimationByName(px_animation *animation,px_char *name);
px_int PX_AnimationGetPlayAnimationIndexByName(px_animation *animation,px_char *name);
px_void PX_AnimationFree(px_animation *animation);
px_void PX_AnimationReset(px_animation *animation);
px_void PX_AnimationUpdate(px_animation *panimation,px_uint elpased);
px_bool PX_AnimationIsEnd(px_animation *panimation);
px_rect PX_AnimationGetSize(px_animation *panimation);
px_void PX_AnimationRender(px_surface *psurface,px_animation *animation,px_point position,PX_TEXTURERENDER_REFPOINT refPoint,PX_TEXTURERENDER_BLEND *blend);


//Create Animation from 2dx data
px_bool PX_AnimationLibraryCreateFromMemory(px_memorypool *mp,px_animationlibrary *panimationLib,px_byte *_2dxBuffer,px_uint size);
px_bool PX_AnimationLibrary_CreateEffect_JumpVertical(px_memorypool *mp,px_animationlibrary *panimation,px_texture *effectTexture);
#endif