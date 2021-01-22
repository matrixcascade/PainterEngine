#ifndef ANDROID_DISPLAY_H
#define ANDROID_DISPLAY_H
//
#include "jni.h"
#include "errno.h"
//opengl es
#include "EGL/egl.h"
#include "GLES3/gl3.h"

//Android Native activity support
#include "android_native_app_glue.h"

#include "../../kernel/PX_Object.h"

typedef  struct
{
    struct android_app *pAndroidApp;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    GLuint vertexShader;
    GLuint pixelsShader;
    GLuint programObject;
    GLuint vPosition;
    GLuint vTex;
    GLuint renderTexture;
    int32_t width;
    int32_t height;

    px_bool PainterEngineReady;
}PX_AndroidEngine;

px_bool  PX_AndroidEngine_InitializeDisplay(PX_AndroidEngine *engine);
px_bool  PX_AndroidEngine_FreeDisplay(PX_AndroidEngine *engine);
px_void PX_AndroidEngine_Render(PX_AndroidEngine* engine,px_int width,px_int height,px_byte *buffer);
#endif