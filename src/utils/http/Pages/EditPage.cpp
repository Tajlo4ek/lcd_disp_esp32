#include "EditPage.h"

#include "utils/fileSystem/FileSystem.h"
#include "utils/wifi/WifiUtils.h"
//#include "Utils/Logger/Logger.h"
#include "utils/FileNames.h"

namespace Pages
{
    EditPage::EditPage(WebServer &server) : Pages::BasePage(server)
    {
        _HTTP->on(F("/edit"), std::bind(&EditPage::Page, this));
        _HTTP->on(F("/edit/getFiles"), std::bind(&EditPage::GetDirList, this));
        _HTTP->on(F("/edit/getFileData"), std::bind(&EditPage::GetFileData, this));
        _HTTP->on(F("/edit/saveFileData"), std::bind(&EditPage::SaveFileData, this));
        _HTTP->on(F("/edit/deleteFile"), std::bind(&EditPage::DeleteFile, this));
    }

    void EditPage::Page()
    {
        _HTTP->send(200, "text/html", FileSystem::ReadFile(EDIT_PAGE_PATH));
    }

    void EditPage::GetDirList()
    {
        String json = GetDirJson(String('/'));
        _HTTP->send(200, F("text/json"), json);
    }

    String EditPage::GetDirJson(const String &path)
    {
        String files = String('[');
        String dirs = String('[');

        auto dir = FileSystem::OpenFile(path, FILE_READ);
        auto file = dir.openNextFile();

        while (file)
        {
            String fileName = file.name();

            if (file.isDirectory())
            {
                dirs += F("{\"name\":\"");
                dirs += fileName;
                dirs += F("\",\"data\":");
                dirs += GetDirJson(fileName);
                dirs += '}';
                dirs += ',';
            }
            else
            {
                files += '"';
                files += fileName.substring(fileName.lastIndexOf('/') + 1);
                files += '"';
                files += ',';
            }
            file = dir.openNextFile();
        }

        if (files[files.length() - 1] == ',')
        {
            files.setCharAt(files.length() - 1, ']');
        }
        else
        {
            files += ']';
        }

        if (dirs[dirs.length() - 1] == ',')
        {
            dirs.setCharAt(dirs.length() - 1, ']');
        }
        else
        {
            dirs += ']';
        }

        String res = F("{\"files\":");
        res += files;
        res += F(",\"dirs\":");
        res += dirs;
        res += '}';

        return res;
    }

    void EditPage::GetFileData()
    {
        String fileName = _HTTP->arg(F("name"));
        _HTTP->send(200, F("text/html"), FileSystem::ReadFile(fileName));
    }

    void EditPage::SaveFileData()
    {
        String fileName = _HTTP->arg(F("name"));
        String fileData = _HTTP->arg(F("data"));

        fileName.trim();

        if (fileName.isEmpty() == false && fileData.isEmpty() == false)
        {
            auto nameLen = fileName.length();
            if (fileName[0] == '/' && nameLen > 2 && nameLen < 32)
            {
                FileSystem::WriteFile(fileName, fileData);
            }
        }

        _HTTP->send(200, F("text/html"), F("Ok"));
    }

    void EditPage::DeleteFile()
    {
        String fileName = _HTTP->arg(F("name"));
        fileName.trim();

        if (fileName.isEmpty() == false)
        {
            auto nameLen = fileName.length();
            if (fileName[0] == '/' && nameLen > 2 && nameLen < 32)
            {
                FileSystem::DeleteFile(fileName);
            }
        }

        _HTTP->send(200, F("text/html"), F("Ok"));
    }

}