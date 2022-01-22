#ifndef _BASE_PAGE_H_
#define _BASE_PAGE_H_

#include <WebServer.h>

namespace Pages
{
    class BasePage
    {
    protected:
        WebServer *_HTTP;

    private:
        virtual void Page() = 0;

    public:
        BasePage(WebServer &server)
        {
            _HTTP = &server;
        }

        virtual const String GetCommand()
        {
            return String();
        }
        
    };
}

#endif