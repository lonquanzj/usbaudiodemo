#pragma once

#include "ILogger.h"

/**
 * Logging class for logging to a file. The actual logging happens on the Java side using the
 * AppendLog() call in ToJava.
 */
class LogFileLogger : public ILogger
{
public:
	LogFileLogger();
	~LogFileLogger();

    void logMessage(const wxString& i_message);
};
