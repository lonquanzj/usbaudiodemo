#pragma once

#include "ILogger.h"

/**
 * Logging class for logging using the Android debugging system. Similar to Log.v() in Java.
 */
class AndroidDebugLogger : public ILogger
{
public:
	AndroidDebugLogger();
	~AndroidDebugLogger();

    void logMessage(const wxString& i_message);
};

