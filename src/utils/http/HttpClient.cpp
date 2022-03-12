#include "HttpClient.h"

#include <WiFiClient.h>

#define CONNECT_WAIT_TIME 3000
namespace HttpClient
{
    String GetHttpContent(const String &host, const String &startLine, const std::vector<std::pair<String, String>> &params, int &code)
    {
        WiFiClient client;
        if (client.connect(host.c_str(), 80) == 0)
        {
            code = 404;
            return "";
        }

        client.print(startLine);
        client.print('?');

        for (const auto &param : params)
        {
            client.print(param.first);
            client.print('=');
            client.print(param.second);
            client.print('&');
        }

        client.println(F(" HTTP/1.1"));
        client.print(F("Host: "));
        client.println(host);
        client.println(F("Connection: close"));
        client.println();

        delay(CONNECT_WAIT_TIME);

        String jsonString;
        while (client.available())
        {
            jsonString += (char)client.read();
        }
        client.stop();

        if (jsonString.isEmpty())
        {
            code = 404;
            return "";
        }

        int startInd = jsonString.indexOf(' ');
        int finishInd = jsonString.indexOf(' ', startInd + 1);
        code = jsonString.substring(startInd, finishInd).toInt();

        return jsonString.substring(jsonString.indexOf(F("\r\n\r\n")) + 4, jsonString.length());
    }
}