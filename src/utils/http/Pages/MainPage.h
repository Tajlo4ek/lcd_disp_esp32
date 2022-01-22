#ifndef _MAIN_PAGE_H_
#define _MAIN_PAGE_H_

#include "BasePage.h"

namespace Pages
{
    class MainPage : public Pages::BasePage
    {
    public:
        MainPage(WebServer &server);

    private:
        void Page() override;
    };
}

#endif