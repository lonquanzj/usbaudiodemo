#pragma once

#include <wx/string.h>

/**
 * Abstract class for logging
 */
class ILogger
{
public:
    ILogger() {};
    virtual ~ILogger() {};

    virtual void logMessage(const wxString& i_message) = 0;
};
