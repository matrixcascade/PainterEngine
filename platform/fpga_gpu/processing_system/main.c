#include "core/PX_Font.h"
#include "core/PX_Surface.h"
#include "core/PX_Texture.h"
#include "core/PX_Typedef.h"
#include "kernel/PX_Kernel.h"
#include "kernel/PX_Object.h"

px_byte *cache=(px_byte *)PX_GPU_RUNTIME_START;
px_memorypool mp;
px_int rotZ=0;
px_texture surface;

px_byte bunny_inflate_data[] = {
#include "bunny.h"
};

PX_Object *particalRoot;
PX_Object* p3DObject;
px_dword switcher;
typedef struct
{
    px_dword alive;
    px_texture ball_tex;
}PX_Object_Ball;

PX_OBJECT_RENDER_FUNCTION(PX_Object_BallRender)
{
    PX_Object_Ball* pDesc = PX_ObjectGetDesc(PX_Object_Ball, pObject);
    PX_TEXTURERENDER_BLEND blend;
    PX_ObjectUpdatePhysics(pObject, elapsed);
    if (pObject->y > 480 - pDesc->ball_tex.height / 2)
    {
        pObject->vy = -PX_ABS(pObject->vy);
    }
    pDesc->alive += elapsed;
    if (pDesc->alive >= 3800)
    {
        PX_ObjectDelayDelete(pObject);
        return;
    }
    blend.hdr_R = 1;
    blend.hdr_G = 1;
    blend.hdr_B = 1;
    blend.alpha = 1.0f-pDesc->alive / 3800.f;
    PX_TextureRender(psurface, &pDesc->ball_tex, (px_int)pObject->x, (px_int)pObject->y, PX_ALIGN_CENTER, &blend);
}

PX_OBJECT_FREE_FUNCTION(PX_Object_Free)
{
    PX_Object_Ball* pDesc = PX_ObjectGetDesc(PX_Object_Ball, pObject);
    PX_TextureFree(&pDesc->ball_tex);

}

PX_Object* PX_Object_BallCreate(px_memorypool *mp,PX_Object* pparent)
{
    PX_Object* pObject;
    px_int size;
    PX_Object_Ball ball;
    size = PX_rand() % 64 + 16;
    ball.alive = 0;
    PX_TextureCreate(mp, &ball.ball_tex, size, size);
    PX_SurfaceClearAll(&ball.ball_tex, PX_COLOR(0, 255, 255, 255));
    PX_GeoDrawBall(&ball.ball_tex, size / 2, size / 2, size / 2 - 8, PX_COLOR(255, PX_rand() & 127 + 128, PX_rand() & 127 + 128, PX_rand() & 127 + 128));
    pObject = PX_ObjectCreateEx(mp, pparent, 400, 480, 0, 0, 0, 0, 0, 0, PX_Object_BallRender, PX_Object_Free, & ball, sizeof(ball));

    pObject->vx = (px_int)(PX_rand() % 600)-300;
    pObject->vy = -PX_sqrt(360000 - pObject->vx * pObject->vx)- (PX_rand() % 600);
    pObject->fy = 600;
    pObject->ak = 0.002f;
    return pObject;
}

PX_OBJECT_EVENT_FUNCTION(MyTimer)
{
    PX_Object_BallCreate(&mp, particalRoot);
}


int main()
{
  
  px_memory objdata;
  PX_3D_ObjectData data;
  px_int i;
  px_int routeY=0;
  mp=MP_Create(cache,1024*1024*64);
   PX_srand(12345);  
  //check GPU available
  if(!PX_GPU_Initialize())
  {
      // initializating GPU error.
      return 0;
  }
  
  PX_TextureCreate(&mp,&surface,PX_GPU_HDMI_OUT_WIDTH,PX_GPU_HDMI_OUT_HEIGHT);
  PX_SurfaceClearAll(&surface,PX_COLOR_WHITE);
  
  //3D testbench
  
  PX_MemoryInitialize(&mp, &objdata);
  PX_RFC1951Inflate(bunny_inflate_data, sizeof(bunny_inflate_data), &objdata);
  PX_MemoryCatByte(&objdata, 0);
  PX_3D_ObjectDataInitialize(&mp, &data);
  if (!PX_3D_ObjectDataLoad(&data, objdata.buffer, objdata.usedsize))return PX_FALSE;

  p3DObject = PX_Object_3DModelCreate(&mp, 0, 400, 240, 480, 480, &data);
  particalRoot=PX_ObjectCreate(&mp,0,0,0,0,0,0,0);
  switcher=0;

  px_dword usUpdate=PX_GPU_TimeGetTime_us();
  PX_Object *timer=PX_Object_TimerCreate(&mp, particalRoot, -1, 50,0);
  PX_ObjectRegisterEvent(timer,PX_OBJECT_EVENT_TIMEOUT, MyTimer,0);
  px_dword FPS=0;
  px_float avgFPS=0;
  px_dword FPStime=0;  
  px_char content[16];
  px_point p1,p2,p3;
  px_point _p1,_p2,_p3;  
  px_float rot=0;
  px_byte gclr=0;
  p1.x=0;
  p1.y=-150;
  p1.z=0;
  p2.x=-150;
  p2.y=150;
  p2.z=0;
  p3.x=150;
  p3.y=150;
  p3.z=0;
  while(1)
  {
    px_dword now=PX_GPU_TimeGetTime_us();
    px_dword elapsed=(now-usUpdate)/1000;
    FPS++;
    switcher++;
    usUpdate=now;
    FPStime+=elapsed;
    if (FPStime>=3000)
    {
       avgFPS=FPS/3.f;
       FPS=0;
       FPStime-=3000;
    }
    
      if (switcher<360)
      {
          PX_SurfaceClearAll(&surface, PX_COLOR(255,255,255,255));
          _p1=PX_PointRotate(p1, rot);
          _p2=PX_PointRotate(p2, rot);
          _p3=PX_PointRotate(p3, rot);
          _p1.x+=400;
          _p1.y+=240;
          _p2.x+=400;
          _p2.y+=240;
          _p3.x+=400;
          _p3.y+=240;
          rot++;
          PX_GeoRasterizeTriangle(&surface, (px_int)_p1.x, (px_int)_p1.y, (px_int)_p2.x, (px_int)_p2.y,\
           (px_int)_p3.x, (px_int)_p3.y, PX_COLOR(128,255-gclr,gclr++,64));
         
      }
      else if(switcher<480)
      {
          PX_SurfaceClearAll(&surface, PX_COLOR_BLACK);
          PX_ObjectUpdate(particalRoot,elapsed);
          PX_ObjectRender(&surface,particalRoot,elapsed);
      }
      else if(switcher<600)
      {
           PX_SurfaceClearAll(&surface, PX_COLOR_WHITE);
          PX_Object_3DModelSetWorld(p3DObject, 0, 0, 1.f, 0, routeY+=2, 0, 1);
          PX_ObjectRender(&surface, p3DObject, 0);
      }
      else
      {
         switcher=0;
      }
      PX_sprintf1(content, 16, "FPS:%1.2", PX_STRINGFORMAT_FLOAT(avgFPS));
      PX_FontDrawText(&surface, 50, 50, PX_ALIGN_CENTER, content, PX_COLOR_RED);
      PX_GPU_Present(surface.surfaceBuffer, PX_GPU_HDMI_OUT_WIDTH, PX_GPU_HDMI_OUT_HEIGHT);
  }
  return 0;
}