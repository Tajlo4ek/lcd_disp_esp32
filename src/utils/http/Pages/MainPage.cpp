#include "MainPage.h"

#include "utils/fileSystem/FileSystem.h"
#include "utils/FileNames.h"

namespace Pages
{
    MainPage::MainPage(WebServer &server) : BasePage::BasePage(server)
    {
        _HTTP->on(String('/'), std::bind(&MainPage::Page, this));
    }

    void MainPage::Page()
    {
        _HTTP->send(200, F("text/html"), FileSystem::ReadFile(INDEX_PAGE_PATH));
    }

}