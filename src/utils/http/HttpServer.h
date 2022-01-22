#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "Arduino.h"
#include <functional>

namespace HttpServer
{
    typedef std::function<String(const String &)> GetCommandCallback;

    void Init(GetCommandCallback callback);

    void HandleServer();

    void AddWebLog(const String &data);
}

#endif