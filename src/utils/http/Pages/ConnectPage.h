#ifndef _CONNECT_PAGE_H_
#define _CONNECT_PAGE_H_

#include "BasePage.h"

namespace Pages
{
    class ConnectPage : public Pages::BasePage
    {
    public:
        ConnectPage(WebServer &server);

    private:
        void Page() override;

        void GetSSID();
        void SetWiFi();
    };
}

#endif