#ifndef _MONITOR_PAGE_H_
#define _MONITOR_PAGE_H_

#include "BasePage.h"

namespace Pages
{
    class MonitorPage : public Pages::BasePage
    {
    public:
        MonitorPage(WebServer &server);

        void AddWebLog(const String &data);

        const String GetCommand() override;

    private:
        void Page() override;

        void GetData();
        void ParseCommand();

        String notSendData;
        String requestCommands;
    };
}

#endif