#ifndef PX_ANIMATION_H
#define PX_ANIMATION_H
#include "PX_2dxCommon.h"
typedef struct
{
	px_vector frames; //vector frames
	px_vector animation;
	px_memory code;
}PX_AnimationLibrary;

typedef struct
{
	px_uint   elapsed;
	px_uint   ip;
	px_uint   reg_loopTimes;
	px_uint   reg_reservedTime;
	px_int    reg_currentFrameIndex;
	px_dword  reg_currentAnimation;
	PX_AnimationLibrary *linker;
}PX_Animation;

typedef struct
{
	px_dword ip;
	px_string name; //image file path
}PX_Animationlibrary_tagInfo;

px_bool PX_AnimationCreate(PX_Animation *animation,PX_AnimationLibrary *linker);
px_bool PX_AnimationSetLibrary(PX_Animation *animation,PX_AnimationLibrary *linker);
px_int PX_AnimationLibraryGetPlayAnimationIndexByName(PX_AnimationLibrary* pLib, const px_char* name);
px_void PX_AnimationLibraryAddInstr(PX_AnimationLibrary *panimationLib,PX_2DX_OPCODE opcode,px_word param);
px_void PX_AnimationLibraryFree(PX_AnimationLibrary *panimationLib);
px_int PX_AnimationGetAnimationsCount(PX_Animation *animation);
px_dword PX_AnimationGetCurrentPlayAnimation(PX_Animation *animation);
px_bool PX_AnimationSetCurrentPlayAnimation(PX_Animation *animation,px_int i);
px_bool PX_AnimationSetCurrentPlayAnimationByName(PX_Animation *animation,const px_char *name);
px_int PX_AnimationGetPlayAnimationIndexByName(PX_Animation* animation, const px_char* name);
px_void PX_AnimationFree(PX_Animation *animation);
px_void PX_AnimationReset(PX_Animation *animation);
px_void PX_AnimationUpdate(PX_Animation *panimation,px_uint elapsed);
px_bool PX_AnimationIsEnd(PX_Animation *panimation);
px_rect PX_AnimationGetSize(PX_Animation *panimation);
px_texture *PX_AnimationGetCurrentTexture(PX_Animation *panimation);
px_void PX_AnimationRenderRotation(px_surface *psurface,PX_Animation *animation,px_point position,px_int angle,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_AnimationRender(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_AnimationRenderEx(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_float scale,px_point direction,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_AnimationRender_scale(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_float scale,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_AnimationRender_vector(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_point direction,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);

//Create Animation from 2dx data
px_int PX_AnimationLibraryGetFrameWidth(PX_AnimationLibrary *panimationLib,px_int frameIndex);
px_int PX_AnimationLibraryGetFrameHeight(PX_AnimationLibrary *panimationLib,px_int frameIndex);
px_bool PX_AnimationLibraryCreateFromMemory(px_memorypool *mp,PX_AnimationLibrary *panimationLib,px_byte *_2dxBuffer,px_uint size);
px_bool PX_AnimationLibrary_CreateEffect_JumpVertical(px_memorypool *mp,PX_AnimationLibrary *panimation,px_texture *effectTexture);
#endif

