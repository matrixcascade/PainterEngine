//PainterEngine framework
#include "PainterEngine_Application.h"


//
#include "jni.h"
#include "errno.h"

//Android Native activity support
#include "android_native_app_glue.h"

#include "platform/modules/px_time.h"
#include "px_display.h"
#include "kernel/PX_Object.h"

static PX_AndroidEngine engine;
volatile px_bool main_exit=0;
static px_int PX_AndroidEngine_Input(struct android_app* app, AInputEvent* event) {
    PX_AndroidEngine* engine = (PX_AndroidEngine*)app->userData;
    px_int32 EventType=AInputEvent_getType(event);
    px_int32 EventCount=AMotionEvent_getPointerCount(event);
    px_int32 action=AMotionEvent_getAction(event);
    px_float cursorx_scale,cursory_scale;
    px_int i;
    px_float x,y;
    PX_Object_Event e;
    if(!engine->PainterEngineReady) return  0;
    cursorx_scale=App.runtime.surface_width*1.0f/App.runtime.window_width;
    cursory_scale=App.runtime.surface_height*1.0f/App.runtime.window_height;
    switch (action&AMOTION_EVENT_ACTION_MASK)
    {
        case AMOTION_EVENT_ACTION_MOVE:
        {
            for (i = 0; i < EventCount; ++i)
            {
                x=AMotionEvent_getX(event, i);
                y=AMotionEvent_getY(event, i);
                e.Event=PX_OBJECT_EVENT_CURSORDRAG;
                PX_Object_Event_SetCursorX(&e,x*cursorx_scale);
                PX_Object_Event_SetCursorY(&e,y*cursory_scale);
                PX_Object_Event_SetCursorIndex(&e,(action&AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>>8);
                PX_ApplicationPostEvent(&App,e);
            }
        }
            break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
        case AMOTION_EVENT_ACTION_DOWN:
        {
            for (i = 0; i < EventCount; ++i)
            {
                x=AMotionEvent_getX(event, i);
                y=AMotionEvent_getY(event, i);
                e.Event=PX_OBJECT_EVENT_CURSORDOWN;
                PX_Object_Event_SetCursorX(&e,x*cursorx_scale);
                PX_Object_Event_SetCursorY(&e,y*cursory_scale);
                PX_Object_Event_SetCursorIndex(&e,(action&AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>>8);
                PX_ApplicationPostEvent(&App,e);
            }
        }
            break;
        case AMOTION_EVENT_ACTION_POINTER_UP:
        case AMOTION_EVENT_ACTION_UP:
        {
            for (i = 0; i < EventCount; ++i)
            {
                x=AMotionEvent_getX(event, i);
                y=AMotionEvent_getY(event, i);
                e.Event=PX_OBJECT_EVENT_CURSORUP;
                PX_Object_Event_SetCursorX(&e,x*cursorx_scale);
                PX_Object_Event_SetCursorY(&e,y*cursory_scale);
                PX_Object_Event_SetCursorIndex(&e,(action&AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>>8);
                PX_ApplicationPostEvent(&App,e);
            }
        }
            break;
    }

    return 0;
}

void PX_AndroidEngine_CMD(struct android_app* app, int32_t cmd)
{
    PX_AndroidEngine* engine = (PX_AndroidEngine*)app->userData;
    switch (cmd)
        {
            case APP_CMD_SAVE_STATE:
                break;
            case APP_CMD_INIT_WINDOW:
                if (engine->pAndroidApp->window != NULL)
                {
                    //PainterEngine Initialize
                    PX_AndroidEngine_InitializeDisplay(engine);
                    if(!engine->PainterEngineReady)
                    {
                        if(!PX_ApplicationInitialize(&App,engine->width,engine->height)) return ;
                        App.runtime.window_width=engine->width;
                        App.runtime.window_height=engine->height;
                    }
                    else
                    {
                        PX_Object_Event e;
                        e.Event=PX_OBJECT_EVENT_WINDOWRESIZE;
                        PX_Object_Event_SetWidth(&e,engine->width);
                        PX_Object_Event_SetHeight(&e,engine->height);
                        PX_ApplicationPostEvent(&App,e);
                    }
                    engine->PainterEngineReady=PX_TRUE;
                }

                break;
            case APP_CMD_TERM_WINDOW:
                PX_AndroidEngine_FreeDisplay(engine);
                break;
            case APP_CMD_GAINED_FOCUS:
                break;
            case APP_CMD_LOST_FOCUS:
                break;
        }
}


ANativeActivity* PX_nativeActivity=PX_NULL;
AAssetManager* PX_assetManager=PX_NULL;

void android_main(struct android_app *pApp)
{

    px_dword lastUpdateTime,elpased;
    int events;
    struct android_poll_source *pSource;
    pApp->userData=&engine;
    pApp->onAppCmd = PX_AndroidEngine_CMD;
    pApp->onInputEvent = PX_AndroidEngine_Input;
    engine.pAndroidApp=pApp;
    PX_nativeActivity = pApp->activity;
    PX_assetManager = PX_nativeActivity->assetManager;


    lastUpdateTime=PX_TimeGetTime();
    do
    {
        while (ALooper_pollAll(0, PX_NULL, &events, (void **) &pSource) >= 0)
        {
            if (pSource)
            {
                pSource->process(pApp, pSource);
                if (pApp->destroyRequested != 0) {
                    PX_AndroidEngine_FreeDisplay(&engine);
                    return;
                }
            }
        }
        if(engine.PainterEngineReady)
        {
            elpased=PX_TimeGetTime()-lastUpdateTime;
            lastUpdateTime=PX_TimeGetTime();
            PX_ApplicationUpdate(&App,elpased);
            PX_ApplicationRender(&App,elpased);
        }

        PX_AndroidEngine_Render(&engine,App.runtime.surface_width,App.runtime.surface_height,(px_byte *)App.runtime.RenderSurface.surfaceBuffer);
    } while (!pApp->destroyRequested&&!main_exit);
}
