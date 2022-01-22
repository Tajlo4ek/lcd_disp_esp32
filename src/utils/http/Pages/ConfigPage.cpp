#include "ConfigPage.h"

#include "utils/fileSystem/FileSystem.h"
#include "utils/FileNames.h"
#include "utils/Commands.h"

namespace Pages
{
    ConfigPage::ConfigPage(WebServer &server) : BasePage::BasePage(server)
    {
        _HTTP->on(F("/editConfig"), std::bind(&ConfigPage::Page, this));
    }

    void ConfigPage::Page()
    {
        String page = FileSystem::ReadFile(CONFIG_PAGE_PATH);

        page.replace(F("{main_screen_config}"), MAIN_SCREEN_CONFIG_PATH);
        page.replace(F("{spectrum_config}"), SPECTRUM_CONFIG_PATH);
        page.replace(F("{reload-command}"), COMMAND_RELOAD_SCREEN);

        _HTTP->send(200, F("text/html"), page);
    }

}