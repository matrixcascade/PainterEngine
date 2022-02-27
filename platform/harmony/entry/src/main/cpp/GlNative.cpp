#include <jni.h>
#include <string>
#include <Hilog/log.h>
#include<iostream>

#include <GLES3/gl32.h>
#include <EGL/egl.h>

//JNI接口环境变量宏定义
#ifndef JNI_KSP_ENGINE
#define JNI_KSP_ENGINE(x) Java_com_PE_demo_slice_MainAbilitySlice_##x
#endif

#ifndef JNI_CLASS_PATH
#define JNI_CLASS_PATH ""
#endif

extern "C"
JNIEXPORT jstring JNICALL
JNI_KSP_ENGINE(stringFromJNI)(JNIEnv* env, jobject  obj) {
    std::string hello = "Hello from JNI C++ codes";
    int len = hello.size();
    jchar res[len];
    for (int i = 0; i < len; i++) {
        res[i] = (jchar)hello[i];
    }
    return env->NewString(res, len);
}

extern "C"
JNIEXPORT void JNICALL
JNI_KSP_ENGINE(onCreate)(JNIEnv* env, jobject  obj) {
//HILOG_DEBUG(LOG_APP,"onDraw  ...../.");

}

extern "C"
JNIEXPORT void JNICALL
JNI_KSP_ENGINE(onDraw)(JNIEnv* env, jobject  obj) {
    //HILOG_DEBUG(LOG_APP,"onDraw  ...../.");
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    //glFlush();
    //glFinish();
}

extern "C"
JNIEXPORT void JNICALL
JNI_KSP_ENGINE(onMouseDown)(JNIEnv* env, jobject  obj,jint x,jint y) {
    HILOG_DEBUG(LOG_APP,"x:%d,y:%d",x,y);



}

extern "C"
JNIEXPORT void JNICALL
JNI_KSP_ENGINE(onMouseMove)(JNIEnv* env, jobject  obj,jint x,jint y) {
    HILOG_DEBUG(LOG_APP,"x:%d,y:%d",x,y);

}
