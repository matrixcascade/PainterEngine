//PainterEngine framework
#include "PainterEngine_Application.h"
//
#include "jni.h"
#include "errno.h"

#include "px_time.h"
#include "na_display.h"
#include "kernel/PX_Object.h"

#include <rawfile/raw_dir.h>
#include <rawfile/raw_file.h>
#include <rawfile/resource_manager.h>

static PX_AndroidEngine engine;

 void onMouseDown(int x,int y){

     unsigned int  *mysurface=(unsigned int  *)malloc(engine.width*engine.height*sizeof(unsigned int ));;
     for(int i=0;i<engine.height;i++)
         for(int j=0;j<engine.width;j++){
             mysurface[j*engine.width+i]=i%255;
         }

     PX_AndroidEngine_Render(&engine,engine.width,engine.height,(unsigned char*)mysurface);
    free(mysurface);
 }
 void onMouseUp(int x,int y){

 }
 void onMouseMove(int x,int y){

 }


//PX_AndroidEngine_FreeDisplay(engine);

ResourceManager * resource_manager=PX_NULL;
void setSurface(NativeLayer *native_Layer,ResourceManager *resourceMgr,EGLint w,EGLint h)
{
    px_dword lastUpdateTime,elpased;
    int events;
    struct android_poll_source *pSource;
    engine.native_Layer=native_Layer;
    resource_manager = resourceMgr;
    PX_AndroidEngine_InitializeDisplay(&engine);

    unsigned int  *mysurface=(unsigned int  *)malloc(w*h*sizeof(unsigned int ));;
    for(int i=0;i<h;i++)
        for(int j=0;j<w;j++){
            mysurface[j*w+i]=i%255;
        }

    PX_AndroidEngine_Render(&engine,w,h,(unsigned char*)mysurface);
    free(mysurface);
}
