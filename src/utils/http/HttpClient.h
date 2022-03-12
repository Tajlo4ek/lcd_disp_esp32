#ifndef _HTTP_HTTP_CLIENT_H_
#define _HTTP_HTTP_CLIENT_H_

#include <Arduino.h>

namespace HttpClient
{
    String GetHttpContent(const String &host, const String &startLine, const std::vector<std::pair<String, String>> &params, int &code);
}
#endif