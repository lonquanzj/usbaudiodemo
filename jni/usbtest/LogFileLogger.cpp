#include "LogFileLogger.h"
#include "ToJava.h"

LogFileLogger::LogFileLogger()
{
}


LogFileLogger::~LogFileLogger()
{
}


void LogFileLogger::logMessage(const wxString& i_message)
{
    AppendLog(i_message);
}

