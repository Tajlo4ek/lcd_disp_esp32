#ifndef _CONFIG_PAGE_H_
#define _CONFIG_PAGE_H_

#include "BasePage.h"

namespace Pages
{
    class ConfigPage : public Pages::BasePage
    {
    public:
        ConfigPage(WebServer &server);

    private:
        void Page() override;
    };
}

#endif