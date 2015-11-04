#ifndef PORTDEFS_H
#define PORTDEFS_H

#include <android/log.h> 

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "libnav", __VA_ARGS__) 
#define wxLogDebug(...) __android_log_print(ANDROID_LOG_DEBUG  , "libnav", __VA_ARGS__)

#define wxLogDebugMain(...) __android_log_print(ANDROID_LOG_DEBUG  , "Main", __VA_ARGS__)
#define wxLogErrorMain(...) __android_log_print(ANDROID_LOG_ERROR  , "Main", __VA_ARGS__)

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , "libnav", __VA_ARGS__) 
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , "libnav", __VA_ARGS__) 
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "libnav", __VA_ARGS__) 
#define wxLogMessage(...) __android_log_print(ANDROID_LOG_DEBUG  , "libnav", __VA_ARGS__)

#define wxLogFuckMain(...) __android_log_print(ANDROID_LOG_ERROR  , "Fuck", __VA_ARGS__)

#endif

