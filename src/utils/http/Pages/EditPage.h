#ifndef _EDIT_PAGE_H_
#define _EDIT_PAGE_H_

#include "BasePage.h"

namespace Pages
{
    class EditPage : public Pages::BasePage
    {
    public:
        EditPage(WebServer &server);

    private:
        void Page() override;

        void GetFileData();
        void SaveFileData();
        void DeleteFile();

        void GetDirList();
        String GetDirJson(const String &path);
    };
}

#endif