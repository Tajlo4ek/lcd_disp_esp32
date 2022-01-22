#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "Arduino.h"

namespace Logger
{
    void SerialLog(const String &data);

    void WebLog(const String &data);

    void Log(const String &data);
}

#endif
