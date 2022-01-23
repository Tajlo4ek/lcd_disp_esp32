#include "HttpServer.h"

#include "WebServer.h"

#include "utils/logger/Logger.h"

#include "pages/ConnectPage.h"
#include "pages/MainPage.h"
#include "pages/MonitorPage.h"
#include "pages/EditPage.h"
#include "pages/ConfigPage.h"

namespace HttpServer
{
    WebServer _HTTP(80);

    Pages::ConnectPage connectPage(_HTTP);
    Pages::MainPage mainPage(_HTTP);
    Pages::MonitorPage monitorPage(_HTTP);
    Pages::EditPage editPage(_HTTP);
    Pages::ConfigPage configPage(_HTTP);

    GetCommandCallback getCommandCallback;

    void Init(GetCommandCallback callback)
    {
        _HTTP.begin();
        getCommandCallback = callback;
    }

    void HandleServer()
    {
        _HTTP.handleClient();
        String command = monitorPage.GetCommand();

        if (command.isEmpty() == false && getCommandCallback != nullptr)
        {
            Logger::WebLog(getCommandCallback(command));
        }
    }

    void AddWebLog(const String &data)
    {
        monitorPage.AddWebLog(data);
    }

}