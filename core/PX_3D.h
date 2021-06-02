#ifndef PX_3D_H
#define PX_3D_H


#include "PX_BaseGeo.h"
#include "PX_Vector.h"
#include "PX_Texture.h"
#include "PX_ImageFilter.h"

#define PX_3D_FACESTATE_ACTIVE 1
#define	PX_3D_FACESTATE_CLIPPED 2
#define	PX_3D_FACESTATE_BACKFACE 4

typedef enum
{
	PX_3D_RENDERLIST_TRANSFORM_LOCAL_TO_LOCAL,
	PX_3D_RENDERLIST_TRANSFORM_LOACL_TO_GLOBAL,
	PX_3D_RENDERLIST_TRANSFORM_GLOBAL_TO_GLOBAL,
}PX_3D_RENDERLIST_TRANSFORM;

typedef enum
{
	PX_3D_CAMERAROTMODE_XYZ,
	PX_3D_CAMERAROTMODE_YXZ,
	PX_3D_CAMERAROTMODE_XZY,
	PX_3D_CAMERAROTMODE_YZX,
	PX_3D_CAMERAROTMODE_ZYX,
	PX_3D_CAMERAROTMODE_ZXY,
}PX_3D_CAMERAROTMODE;

typedef enum
{
	PX_3D_CULLMODE_NONE,
	PX_3D_CULLMODE_CW,
	PX_3D_CULLMODE_CCW,
}PX_3D_CULLMODE;

#define PX_3D_PRESENTMODE_POINT   1
#define PX_3D_PRESENTMODE_LINE    2
#define PX_3D_PRESENTMODE_TEXTURE 4
#define PX_3D_PRESENTMODE_PURE	  8

typedef px_void (*PX_3D_PixelShader)(px_surface *psurface,px_int x,px_int y,px_point position,px_float u,px_float v,px_point4D normal,px_texture *pTexture,px_color color);



typedef struct
{
	px_point4D  position;
	px_vector4D normal;
	px_color    clr;
	px_float    u,v;
}PX_3D_Vertex;

typedef struct 
{
	px_dword    state;
	PX_3D_Vertex  vertex[3];
	PX_3D_Vertex  transform_vertex[3];
}PX_3D_Face;

typedef struct
{
	px_memorypool *mp;
	px_texture *ptexture;
	px_vector facestream;
	px_dword PX_3D_PRESENTMODE;
	PX_3D_CULLMODE cullmode;
	PX_3D_PixelShader pixelShader;
}PX_3D_RenderList;

typedef struct
{
	px_matrix mat;
}PX_3D_World;

typedef enum
{
	PX_3D_CAMERA_TYPE_EULER,
	PX_3D_CAMERA_TYPE_UVN,
}PX_3D_CAMERA_TYPE;

typedef struct  
{
	PX_3D_CAMERA_TYPE type;
	px_memorypool *mp;
	px_float near_z,far_z;
	
	px_float fov;
	px_float d;

	px_point4D position;
	union
	{
	px_point4D targetDirection;
    px_point4D rotDirection;
	};
	px_float viewplane_width,viewplane_height;
	px_float viewport_width,viewport_height,viewport_center_x,viewport_center_y;
	px_float aspect_ratio;

	px_matrix mat_cam,mat_per,mat_scr;

	px_float *zbuffer;
}PX_3D_Camera;



px_void PX_3D_WorldInitialize(PX_3D_World *world,px_float x,px_float y,px_float z,px_float rotX,px_float rotY,px_float rotZ,px_float scale);
px_bool PX_3D_CameraEulerInitialize(px_memorypool *mp,PX_3D_Camera *camera,px_point4D cameraPosition,px_float rotX,px_float rotY,px_float rotZ,px_float near,px_float far,px_float fov,px_float viewPortWidth,px_float viewPortHeight);
px_bool PX_3D_CameraUVNInitialize(px_memorypool *mp,PX_3D_Camera *camera,px_point4D cameraPosition,px_point4D cameraTarget,px_float near,px_float far,px_float fov,px_float viewPortWidth,px_float viewPortHeight);
px_void PX_3D_CameraSetPosition(PX_3D_Camera *camera,px_point4D cameraPosition,px_point4D cameraTarget);
px_bool PX_3D_RenderListInitialize(px_memorypool *mp,PX_3D_RenderList *list,px_dword PX_3D_PRESENTMODE,PX_3D_CULLMODE cullmode,px_texture *ptexture);
px_void PX_3D_CameraFree(PX_3D_Camera *camera);
px_bool PX_3D_RenderListPush(PX_3D_RenderList *list,PX_3D_Face face);
px_point *PX_3D_CreateTextureNormal(px_memorypool *mp,px_texture *pTexture);
px_point *PX_3D_CreateBumpTextureNormal(px_memorypool *mp,px_texture *pTexture);
px_void PX_3D_RenderListTransform(PX_3D_RenderList *list,px_matrix mat,PX_3D_RENDERLIST_TRANSFORM t);
px_void PX_3D_RenderListReset(PX_3D_RenderList *list);
px_void PX_3D_RenderListFree(PX_3D_RenderList *list);
px_void PX_3D_Scene(PX_3D_RenderList *list,PX_3D_World *world,PX_3D_Camera *camera);
px_void PX_3D_Present(px_surface *psurface, PX_3D_RenderList *list,PX_3D_Camera *camera);
px_void PX_3D_RenderListSetPixelShader(PX_3D_RenderList *list,PX_3D_PixelShader func);



#endif
