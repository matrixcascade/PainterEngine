#include "PainterEngine_Application.h"

PX_Application App;

px_surface _3drenderTarget;
PX_3D_RenderList renderlist;
PX_3D_Camera camera;
PX_3D_World world;
PX_3D_ObjectData Obj;
PX_IO_Data io;
px_float angle=0; 

px_void PX_APP_3D_PixelShader(px_surface *psurface,px_int x,px_int y,px_point position,px_float u,px_float v,px_point4D normal,px_texture *pTexture,px_color color)
{
	px_float alpha;
	px_float cosv=PX_Point4DDot(PX_Point4DUnit(normal),PX_POINT4D(0,0,1));

	cosv=-cosv;
	if (cosv>0)
	{
		alpha=(1-cosv)*128;
		PX_SurfaceDrawPixel(psurface,x,y,PX_COLOR(255,(px_uchar)(128+alpha),(px_uchar)(128+alpha),(px_uchar)(128+alpha)));
	}
}

px_bool PX_ApplicationInitialize(PX_Application *pApp,px_int screen_width,px_int screen_height)
{
	PX_ApplicationInitializeDefault(&pApp->runtime, screen_width, screen_height);
	PX_SurfaceCreate(&pApp->runtime.mp_game,600,600,&_3drenderTarget);
	PX_3D_ObjectDataInitialize(&pApp->runtime.mp_game,&Obj);
	io=PX_LoadFileToIOData("assets/bunny.obj");
	if(io.size==0)return PX_FALSE;
	if(!PX_3D_ObjectDataLoad(&Obj,io.buffer,io.size))return PX_FALSE;

	PX_3D_RenderListInitialize(&pApp->runtime.mp_game,&renderlist,PX_3D_PRESENTMODE_TEXTURE|PX_3D_PRESENTMODE_PURE,PX_3D_CULLMODE_CCW,PX_NULL);
	PX_3D_RenderListSetPixelShader(&renderlist,PX_APP_3D_PixelShader);
	PX_3D_ObjectDataToRenderList(&Obj,&renderlist);
	PX_3D_CameraUVNInitialize(&pApp->runtime.mp_game,&camera,PX_POINT4D(0,0,0),PX_POINT4D(0,0,1),1,999,90,600,600);
	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elpased)
{

}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elpased)
{
	px_surface *pRenderSurface=&pApp->runtime.RenderSurface;
	PX_RuntimeRenderClear(&pApp->runtime,PX_COLOR(255,255,255,255));
	PX_SurfaceClear(&_3drenderTarget,0,0,_3drenderTarget.width-1,_3drenderTarget.height-1,PX_COLOR(255,255,255,255));
	PX_3D_WorldInitialize(&world,0,0,1.2f,0,angle++,0,1);
	PX_3D_Scene(&renderlist,&world,&camera);
	PX_3D_Present(&_3drenderTarget,&renderlist,&camera);
	PX_SurfaceRender(pRenderSurface,&_3drenderTarget,pRenderSurface->width/2,pRenderSurface->height/2,PX_ALIGN_CENTER,PX_NULL);
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	PX_ApplicationEventDefault(&pApp->runtime, e);

}

