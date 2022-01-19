#ifndef __PX_LOG_H
#define __PX_LOG_H

#if defined(ANDROID)
#include <android/log.h>
#define  LOG_TAG    "main"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define printf LOGE
#endif
/*
#if defined(HARMONY)
#include <Hilog/log.h>
#define  LOGI(...) HILOG_INFO(LOG_APP,__VA_ARGS__)
#define  LOGD(...) HILOG_DEBUG(LOG_APP,__VA_ARGS__)
#define  LOGE(...) HILOG_ERROR(LOG_APP,__VA_ARGS__)
#define printf LOGE
#endif
*/
void PX_LOG(char fmt[]);
void PX_ERROR(char fmt[]);
char *PX_GETLOG(void);
void PX_ASSERT(void);

#endif
