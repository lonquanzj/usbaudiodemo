#include "AndroidDebugLogger.h"
#include <android/log.h>


AndroidDebugLogger::AndroidDebugLogger()
{
}


AndroidDebugLogger::~AndroidDebugLogger()
{
}


void AndroidDebugLogger::logMessage(const wxString& i_message)
{
    __android_log_print(ANDROID_LOG_DEBUG, "eXtream", "%s", i_message.c_str());
}

