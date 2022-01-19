
#include <jni.h>
#include <string>
#include <Hilog/log.h>
#include<iostream>
//
#include <GLES3/gl32.h>
#include <EGL/egl.h>

//JNI接口环境变量宏定义
#ifndef JNI_KSP_ENGINE
#define JNI_KSP_ENGINE(x) Java_com_PE_EngineDemo_slice_MainAbilitySlice_##x
#endif

#ifndef JNI_CLASS_PATH
#define JNI_CLASS_PATH ""
#endif

#define  LOGD(...) HILOG_DEBUG(LOG_APP,__VA_ARGS__)
#define  LOGE(...) HILOG_ERROR(LOG_APP,__VA_ARGS__)

#include <GLES3/gl3platform.h>
//#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <harea.h>
#include <native_layer.h>
#include <native_layer_jni.h>

#include <multimedia/image/image_pixel_map.h>

#include <rawfile/raw_dir.h>
#include <rawfile/raw_file.h>
#include <rawfile/resource_manager.h>
#include <appexecfwk/native_interface_eventhandler.h>


NativeLayer *native_Layer=nullptr;
ResourceManager * resourceManager=nullptr;

JNIEnv *genv=nullptr;
JavaVM *jvm;
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    void* env;
    jvm=vm;

    if (vm->GetEnv( &env, JNI_VERSION_1_4) != JNI_OK)
    {
        LOGE("Failed to get the environment using GetEnv()");
        return -1;
    }
    jvm->AttachCurrentThread(&env, nullptr);
    genv=(JNIEnv *)env;
    return JNI_VERSION_1_4;
}


void javaEnvTest(){


}

EGLConfig	            m_eglConfig = NULL;
EGLDisplay	            m_eglDisplay = EGL_NO_DISPLAY;
EGLContext	            m_eglContext = EGL_NO_CONTEXT;
EGLSurface	            m_eglWindowSurface = EGL_NO_SURFACE;
EGLint SCRW, SCRH, SCR_format;

void initEGL(NativeLayer *native_Layer,EGLint w,EGLint h,EGLint format){

    //eglConfig
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_SAMPLES,4,
        EGL_CONFORMANT,EGL_OPENGL_ES3_BIT,
        EGL_RENDERABLE_TYPE,EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };
    EGLint numConfigs;
    EGLConfig config = nullptr;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(display)
    {
        EGLint mvm=0;EGLint mvv=0;
        eglInitialize(display, &mvm, &mvv);
        m_eglDisplay=display;
    }

    eglChooseConfig(display, attribs, nullptr,0, &numConfigs);
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
    assert(supportedConfigs);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);
    assert(numConfigs);
    auto i = 0;
    for (; i < numConfigs; i++)
    {
        auto& cfg = supportedConfigs[i];
        EGLint r, g, b, d;
        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r)   &&
                eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
                eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b)  &&
                eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
                r == 8 && g == 8 && b == 8 && d == 0 )
        {

            config = supportedConfigs[i];
            break;
        }
    }
    if (i == numConfigs)
    {
        config = supportedConfigs[0];
    }

    if (config != nullptr)
    {
        m_eglConfig=config;
    }
    if(m_eglDisplay&&m_eglConfig)
    {
        const EGLint ctxAttr[]={EGL_CONTEXT_CLIENT_VERSION, 3,EGL_NONE};
        m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, nullptr, ctxAttr);
    }
    auto surface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, native_Layer, nullptr);
    if(surface!=EGL_NO_SURFACE)
    {
        m_eglWindowSurface=surface;
    }
    //绑定EglContext和Surface到显示设备中
    if (!eglMakeCurrent(m_eglDisplay, m_eglWindowSurface, m_eglWindowSurface, m_eglContext)) {
        LOGE("eglMakeCurrent fail");
    }

    glViewport(0, 0, w, h);
}

extern "C"
JNIEXPORT jstring JNICALL
JNI_KSP_ENGINE(stringFromJNI)(JNIEnv* env, jobject  obj) {
    std::string hello = "Hello from JNI C++ codes";
    int len = hello.size();
    jchar res[len];
    for (int i = 0; i < len; i++) {
        res[i] = (jchar)hello[i];
    }
    jstring l;

    return genv->NewString(res, len);
}

extern void setSurface(NativeLayer *native_Layer,ResourceManager *resourceMgr,EGLint w,EGLint h);
extern "C"
JNIEXPORT void JNICALL
JNI_KSP_ENGINE(setSurface)(JNIEnv* env, jobject  obj,jobject jsurface,jobject resourceMgr) {
    HILOG_DEBUG(LOG_APP,"onDraw  ...../.");

    native_Layer=GetNativeLayer(env,jsurface);
    SCRW=NativeLayerHandle(native_Layer,GET_WIDTH);
    SCRH=NativeLayerHandle(native_Layer,GET_HEIGHT);
    SCR_format=NativeLayerHandle(native_Layer,GET_FORMAT);
    //initEGL(native_Layer,SCRW,SCRH,SCR_format);

    resourceManager=InitNativeResourceManager(env, resourceMgr);
    setSurface(native_Layer,resourceManager,SCRW,SCRH);
}

extern "C"
JNIEXPORT void JNICALL
JNI_KSP_ENGINE(onCreate)(JNIEnv* env, jobject  obj) {
    HILOG_DEBUG(LOG_APP,"onDraw  ...../.");

    javaEnvTest();

}

extern "C"
JNIEXPORT void JNICALL
JNI_KSP_ENGINE(onDraw)(JNIEnv* env, jobject  obj) {
    //HILOG_DEBUG(LOG_APP,"onDraw  ...../.");
//    glClearColor(1.0, 0.0, 0.0, 1.0);
//    glClear(GL_COLOR_BUFFER_BIT);
    //glFlush();
    //glFinish();
}

extern void onMouseDown(int x,int y);
extern void onMouseUp(int x,int y);
extern void onMouseMove(int x,int y);

extern "C"
JNIEXPORT void JNICALL
JNI_KSP_ENGINE(onMouseDown)(JNIEnv* env, jobject  obj,jint x,jint y) {
    HILOG_DEBUG(LOG_APP,"x:%d,y:%d",x,y);

    LOGE("cocos2d-x assert", "%s function:%s line:%d",    __FILE__, __FUNCTION__, __LINE__);
    if(native_Layer!=nullptr){

//        NativeLayerBuffer nativeLayerBuffer;
//        void* buf=nativeLayerBuffer.buffer_bits;

//        glClearColor(1.0, 0.0, 0.0, 1.0);
//        glClear(GL_COLOR_BUFFER_BIT);
//        eglSwapBuffers(m_eglDisplay, m_eglWindowSurface);
//        NativeLayerHandle(native_Layer,DRAW_LAYER);
        LOGE("hhhhh %d , %d ,%d , %d ",NativeLayerHandle(native_Layer,GET_WIDTH), NativeLayerHandle(native_Layer,GET_HEIGHT)
//            ,nativeLayerBuffer.buffer_width,nativeLayerBuffer.buffer_height
        );
        onMouseDown( x, y);
    }

}

extern "C"
JNIEXPORT void JNICALL
JNI_KSP_ENGINE(onMouseMove)(JNIEnv* env, jobject  obj,jint x,jint y) {
//    HILOG_DEBUG(LOG_APP,"x:%d,y:%d",x,y);
    onMouseMove( x, y);
}

extern "C"
JNIEXPORT void JNICALL
JNI_KSP_ENGINE(onMouseUp)(JNIEnv* env, jobject  obj,jint x,jint y) {
//    HILOG_DEBUG(LOG_APP,"x:%d,y:%d",x,y);
    onMouseUp( x, y);
}
