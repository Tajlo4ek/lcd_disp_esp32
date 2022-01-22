
#include "Logger.h"
#include "utils/http/HttpServer.h"
#include "utils/Commands.h"

namespace Logger
{
    String BuildData(const String &data);

    void SerialLog(const String &data)
    {
        Serial.println(BuildData(data));
    }

    void WebLog(const String &data)
    {
        HttpServer::AddWebLog(BuildData(data));
    }

    void Log(const String &data)
    {
        //SerialLog(data);
        WebLog(data);
    }

    String BuildData(const String &data)
    {
        String bData = LOG_TOKEN;
        bData += millis();
        bData += ':';
        bData += ' ';
        bData += data;
        bData += COMMAND_STOP_CHAR;

        return bData;
    }

}
