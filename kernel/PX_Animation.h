#ifndef PX_ANIMATION_H
#define PX_ANIMATION_H
#include "PX_2dxCommon.h"
#include "PX_Lexer.h"
typedef struct
{
	px_memorypool* mp;
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
	px_int	  reg_clipx;
	px_int	  reg_clipy;
	px_int	  reg_clipw;
	px_int	  reg_cliph;
	px_int	  reg_clipi;
	px_int	  reg_priority;
	PX_AnimationLibrary *linker;
}PX_Animation;

typedef struct
{
	px_dword ip;
	px_string name; //image file path
}PX_Animationlibrary_tagInfo;

typedef struct
{
	px_char path[64]; //image file path
	px_char map[64];
}PX_2dxMake_textureInfo;

typedef struct
{
	px_char tag[64]; //animation tag
	px_dword addr;
}PX_2dxMake_tagInfo;

//Create Animation from 2dx data
px_int PX_AnimationLibraryGetFrameWidth(PX_AnimationLibrary* panimationLib, px_int frameIndex);
px_int PX_AnimationLibraryGetFrameHeight(PX_AnimationLibrary* panimationLib, px_int frameIndex);
px_bool PX_AnimationLibraryCreateFromMemory(px_memorypool* mp, PX_AnimationLibrary* panimationLib, px_byte* _2dxBuffer, px_uint size);
px_bool PX_AnimationLibrary_CreateEffect_JumpVertical(px_memorypool* mp, PX_AnimationLibrary* panimation, px_texture* effectTexture);
px_int PX_AnimationLibraryGetPlayAnimationIndexByName(PX_AnimationLibrary* pLib, const px_char* name);
px_void PX_AnimationLibraryAddInstr(PX_AnimationLibrary *panimationLib,PX_2DX_OPCODE opcode,px_word param);
px_void PX_AnimationLibraryFree(PX_AnimationLibrary *panimationLib);


px_bool PX_AnimationIsActivity(PX_Animation* animation);
px_bool PX_AnimationCreate(PX_Animation* animation, PX_AnimationLibrary* linker);
px_bool PX_AnimationSetLibrary(PX_Animation* animation, PX_AnimationLibrary* linker);
px_int PX_AnimationGetAnimationsCount(PX_Animation *animation);
px_bool PX_AnimationGetRenderRange(PX_Animation* animation,px_recti *range);
px_dword PX_AnimationGetCurrentPlayAnimation(PX_Animation *animation);
const px_char* PX_AnimationGetCurrentPlayAnimationName(PX_Animation* animation);
px_bool PX_AnimationSetCurrentPlayAnimation(PX_Animation *animation,px_int i);
px_bool PX_AnimationPlay(PX_Animation* animation, const px_char* name);
px_bool PX_AnimationPlayWithPriority(PX_Animation* animation, const px_char* name, px_int Priority);
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
px_void PX_AnimationRenderMirror(px_surface* psurface, PX_Animation* animation, px_int x, px_int y, PX_ALIGN refPoint, PX_TEXTURERENDER_BLEND* blend, PX_TEXTURERENDER_MIRRROR_MODE mirror);
px_void PX_AnimationRenderEx(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_float scale,px_point2D direction,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_AnimationRender_scale(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_float scale,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_void PX_AnimationRender_vector(px_surface *psurface,PX_Animation *animation,px_int x,px_int y,px_point2D direction,PX_ALIGN refPoint,PX_TEXTURERENDER_BLEND *blend);
px_int PX_AnimationGetFrameWidth(PX_Animation *animation);
px_int PX_AnimationGetFrameHeight(PX_Animation *animation);


//compiler
px_bool PX_AnimationShellCompile(px_memorypool* mp, const px_char script[], px_vector* taginfos, px_vector* texinfos, px_memory* out);

#endif

