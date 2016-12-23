#ifndef _APPLOG_H_
#define _APPLOG_H_

#include <android/log.h>

#if !defined(LOG_TAG)
#define LOG_TAG "3D_ENGINE_TEST"
#endif

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif /* _APPLOG_H_ */

