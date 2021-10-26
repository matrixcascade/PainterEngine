#ifndef PX_LIVEFRAMEWORK_H
#define PX_LIVEFRAMEWORK_H

#include "../core/PX_Core.h"

#define PX_LIVEFRAMEWORK_MAX_SUPPORT_LAYER 256
#define PX_LIVE_ID_MAX_LEN 32
#define PX_LIVE_LAYER_MAX_LINK_NODE 16
#define PX_LIVE_VERSION 0x00000001
typedef enum
{
	PX_LIVE_ANIMATION_FRAME_OPCODE_STAMP=0,
	PX_LIVE_ANIMATION_FRAME_OPCODE_TRANSLATION,
}PX_LIVE_ANIMATION_FRAME_OPCODE;

//////////////////////////////////////////////////////////////////////////
//live base struct
typedef struct
{
	px_point	sourcePosition;
	px_point    currentPosition;
	px_point	normal;
	px_point	beginTranslation;//point
	px_point	currentTranslation;//point
	px_point	endTranslation;//point
	px_point    velocity;
	px_int32	k;
	px_float32  u,v;
}PX_LiveVertex;

typedef struct
{
	px_int index1;
	px_int index2;
	px_int index3;
}PX_LiveTriangle;

typedef struct  
{
	px_char id[PX_LIVE_ID_MAX_LEN];
	px_texture Texture;
	px_int textureOffsetX;
	px_int textureOffsetY;
}PX_LiveTexture;

//////////////////////////////////////////////////////////////////////////
//live animation
typedef struct
{
	px_char id[PX_LIVE_ID_MAX_LEN];
	px_vector framesMemPtr;//px_void *
}PX_LiveAnimation;
//////////////////////////////////////////////////////////////////////////
//shell codes struct

typedef struct
{
	px_char frameid[PX_LIVE_ID_MAX_LEN];
	px_int32 size;
	px_uint32 duration_ms;
}PX_LiveAnimationFrameHeader;

//////////////////////////////////////////////////////////////////////////

typedef struct  
{
	px_point32 translation;//only root layer
	px_float32 stretch;//only child layer
	px_float32 rotation;
	px_int32 mapTexture;
	px_int32 translationVerticesCount;
	px_point32 impulse;

	px_float panc_x, panc_y, panc_width, panc_height;
	px_float panc_sx,panc_sy;
	px_float panc_endx, panc_endy;
	px_dword reserve[32];
}PX_LiveAnimationFramePayload;
//////////////////////////////////////////////////////////////////////////



struct _PX_LiveLayer
{
	px_char id[PX_LIVE_ID_MAX_LEN];

	px_int32 parent_index;
	px_int32 child_index[PX_LIVE_LAYER_MAX_LINK_NODE];


	//second Transform
	px_float rotationAngle;
	px_float rel_beginRotationAngle;
	px_float rel_currentRotationAngle;
	px_float rel_endRotationAngle;
	
	//third Transform
	px_float rel_beginStretch;
	px_float rel_currentStretch;
	px_float rel_endStretch;

	px_point rel_beginTranslation;
	px_point rel_currentTranslation;
	px_point rel_endTranslation;
	
	//impulse
	px_point rel_impulse;

	//keyPoint
	px_point keyPoint;
	px_point currentKeyPoint;

	//vertices
	px_vector vertices;//PX_LiveVertex
	//triangle
	px_vector triangles;//PX_LiveTriangle

	//panc
	px_float panc_x, panc_y, panc_width, panc_height, panc_sx, panc_sy;
	px_float panc_beginx, panc_beginy;
	px_float panc_currentx, panc_currenty;
	px_float panc_endx, panc_endy;

	px_bool visible;
	px_bool showMesh;
	px_int  LinkTextureIndex;
	px_int  RenderTextureIndex;
};
typedef struct _PX_LiveLayer PX_LiveLayer;

//////////////////////////////////////////////////////////////////////////
typedef px_void (*PX_LiveFramework_PixelShader)(px_surface *psurface,px_int x,px_int y,px_point position,px_float u,px_float v,px_point normal,px_texture *pTexture,PX_TEXTURERENDER_BLEND *blend);

typedef enum
{
	PX_LIVEFRAMEWORK_STATUS_STOP,
	PX_LIVEFRAMEWORK_STATUS_PLAYING,
}PX_LIVEFRAMEWORK_STATUS;

typedef struct
{
	px_memorypool *mp;
	//////////////////////////////////////////////////////////////////////////
	//attributes
	px_char id[PX_LIVE_ID_MAX_LEN];
	
	//////////////////////////////////////////////////////////////////////////
	//controller
	px_int currentEditLayerIndex;
	px_int currentEditAnimationIndex;
	px_int currentEditFrameIndex;
	px_int currentEditVertexIndex;

	//////////////////////////////////////////////////////////////////////////
	//render
	px_vector layers;
	px_vector livetextures;
	px_vector liveAnimations;
	px_int width;
	px_int height;
	px_bool showRange;
	px_bool showKeypoint;
	px_bool showlinker;
	px_bool showFocusLayer;
	px_bool showRootHelperLine;
	px_float refer_x,refer_y;
	PX_LIVEFRAMEWORK_STATUS status;
	PX_LiveFramework_PixelShader pixelShader;

	//////////////////////////////////////////////////////////////////////////
	//VirtualMachine
	px_int32 reg_duration;
	px_int32 reg_ip;
	px_int32 reg_elapsed;
	px_int32 reg_animation;
	px_int32 reg_bp;
}PX_LiveFramework;



px_bool PX_LiveFrameworkInitialize(px_memorypool *mp,PX_LiveFramework *plive,px_int width,px_int height);
px_void PX_LiveFrameworkPlay(PX_LiveFramework *plive);
px_void PX_LiveFrameworkPause(PX_LiveFramework *plive);
px_void PX_LiveFrameworkReset(PX_LiveFramework *plive);
px_void PX_LiveFrameworkStop(PX_LiveFramework *plive);


px_void PX_LiveFrameworkRender(px_surface *psurface,PX_LiveFramework *plive,px_int x,px_int y,PX_ALIGN refPoint,px_dword elapsed);
px_void PX_LiveFrameworkRenderRefer(px_surface *psurface,PX_LiveFramework *plive,PX_ALIGN refPoint,px_dword elapsed);

px_bool PX_LiveFrameworkPlayAnimation(PX_LiveFramework *plive,px_int index);
px_bool PX_LiveFrameworkPlayAnimationByName(PX_LiveFramework *plive,const px_char name[]);
PX_LiveLayer *PX_LiveFrameworkGetLayerById(PX_LiveFramework *plive,const px_char id[]);
PX_LiveLayer *PX_LiveFrameworkCreateLayer(PX_LiveFramework *plive,const px_char id[]);
PX_LiveLayer *PX_LiveFrameworkGetLayerParent(PX_LiveFramework *plive,PX_LiveLayer *pLayer);
PX_LiveLayer *PX_LiveFrameworkGetLayerChild(PX_LiveFramework *plive,PX_LiveLayer *pLayer,px_int childIndex);

px_bool PX_LiveFrameworkLinkLayerTexture(PX_LiveFramework *plive,const px_char layer_id[],const px_char texture_id[]);
PX_LiveLayer *PX_LiveFrameworkGetLayer(PX_LiveFramework *plive,px_int index);
px_int PX_LiveFrameworkGetLayerIndex(PX_LiveFramework *plive,PX_LiveLayer *pLayer);
PX_LiveLayer *PX_LiveFrameworkGetLastCreateLayer(PX_LiveFramework *plive);

px_void PX_LiveFrameworkUpdateLayerSourceVerticesUV(PX_LiveFramework *plive,PX_LiveLayer *pLayer);
px_void PX_LiveFrameworkUpdateSourceVerticesUV(PX_LiveFramework *plive);
px_void PX_LiveFrameworkUpdateLayerRenderVerticesUV(PX_LiveFramework *plive,PX_LiveLayer *pLayer);

PX_LiveAnimation *PX_LiveFrameworkGetAnimationById(PX_LiveFramework *plive,const px_char id[]);
PX_LiveAnimation *PX_LiveFrameworkCreateAnimation(PX_LiveFramework *plive,const px_char id[]);
PX_LiveAnimation *PX_LiveFrameworkGetAnimation(PX_LiveFramework *plive,px_int index);
PX_LiveAnimation *PX_LiveFrameworkGetLastCreateAnimation(PX_LiveFramework *plive);

px_bool PX_LiveFrameworkAddLiveTexture(PX_LiveFramework *plive,PX_LiveTexture livetexture);
PX_LiveTexture *PX_LiveFrameworkGetLiveTextureById(PX_LiveFramework *plive,const px_char id[]);
px_int PX_LiveFrameworkGetLiveTextureIndexById(PX_LiveFramework *plive,const px_char id[]);
PX_LiveTexture *PX_LiveFrameworkGetLiveTexture(PX_LiveFramework *plive,px_int index);
px_void  PX_LiveFrameworkDeleteLiveTextureById(PX_LiveFramework *plive,const px_char id[]);
px_void PX_LiveFrameworkDeleteLiveAnimationById(PX_LiveFramework *plive,const px_char id[]);
px_bool PX_LiveFrameworkLinkLayerSearchSubLayer(PX_LiveFramework *plive,PX_LiveLayer *pLayer,PX_LiveLayer *pSearchLayer);
px_void PX_LiveFrameworkLinkLayer(PX_LiveFramework *plive,PX_LiveLayer *pLayer,PX_LiveLayer *linkLayer);
px_void PX_LiveFrameworkClearLinker(PX_LiveFramework *plive);
px_void PX_LiveFrameworkDeleteLayer(PX_LiveFramework *plive,px_int index);
px_void PX_LiveFrameworkDeleteLiveTexture(PX_LiveFramework *plive,px_int index);
px_void PX_LiveFrameworkDeleteLiveAnimation(PX_LiveFramework *plive,px_int index);
px_void PX_LiveFrameworkDeleteLiveAnimationFrameByIndex(PX_LiveFramework *plive,px_int AnimationIndex,px_int frameIndex);
px_void PX_LiveFrameworkFree(PX_LiveFramework *plive);


//////////////////////////////////////////////////////////////////////////
//edit functions
px_void *PX_LiveFrameworkGetCurrentEditFrame(PX_LiveFramework *plive);
PX_LiveLayer *PX_LiveFrameworkGetCurrentEditLiveLayer(PX_LiveFramework *plive);
PX_LiveVertex *PX_LiveFrameworkGetCurrentEditLiveVertex(PX_LiveFramework *plive);

px_void PX_LiveFrameworkCurrentEditMoveFrameDown(PX_LiveFramework *plive);
px_void PX_LiveFrameworkCurrentEditMoveFrameUp(PX_LiveFramework *plive);

PX_LiveAnimation * PX_LiveFrameworkGetCurrentEditAnimation(PX_LiveFramework *plive);
PX_LiveAnimationFrameHeader * PX_LiveFrameworkGetCurrentEditAnimationFrame(PX_LiveFramework *plive);
PX_LiveAnimationFramePayload *PX_LiveFrameworkGetCurrentEditAnimationFramePayloadIndex(PX_LiveFramework *plive,px_int payloadIndex);
PX_LiveAnimationFramePayload *PX_LiveFrameworkGetCurrentEditAnimationFramePayload(PX_LiveFramework *plive);
px_point *PX_LiveFrameworkGetCurrentEditAnimationFramePayloadVertex(PX_LiveFramework *plive);
px_void PX_LiveFrameworkDeleteCurrentEditAnimation(PX_LiveFramework *plive);
px_void PX_LiveFrameworkDeleteCurrentEditAnimationFrame(PX_LiveFramework *plive);
px_bool PX_LiveFrameworkNewEditFrame(PX_LiveFramework *plive,px_char id[],px_bool bCopyFrame);
px_void PX_LiveFrameworkRunCurrentEditFrame(PX_LiveFramework *plive);

px_bool PX_LiveFrameworkExport(PX_LiveFramework *plive,px_memory *exportbuffer);
px_bool PX_LiveFrameworkImport(px_memorypool *mp,PX_LiveFramework *plive,px_void *buffer,px_int size);


//////////////////////////////////////////////////////////////////////////
//mirror
typedef PX_LiveFramework PX_Live;
px_bool PX_LiveCreate(px_memorypool *mp,PX_LiveFramework *pLiveFramework,PX_Live *pLive);
px_void PX_LiveFree(PX_Live *pLive);

px_void PX_LivePlay(PX_Live*plive);
px_bool PX_LivePlayAnimation(PX_Live *plive,px_int index);
px_bool PX_LivePlayAnimationByName(PX_Live *plive,const px_char name[]);
px_void PX_LivePause(PX_Live *plive);
px_void PX_LiveReset(PX_Live *plive);
px_void PX_LiveStop(PX_Live *plive);

px_void PX_LiveRender(px_surface *psurface,PX_Live *plive,px_int x,px_int y,PX_ALIGN refPoint,px_dword elapsed);


#endif
