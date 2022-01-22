#include "FileSystem.h"

#include "utils/FileNames.h"

#define CONFIG_LITTLEFS_SPIFFS_COMPAT 1

namespace FileSystem
{
    bool isInit = false;

    void Init()
    {
        LITTLEFS.begin();

        LITTLEFS.mkdir(CONFIG_DIR);
        LITTLEFS.mkdir(PAGES_DIR);

        isInit = true;
    }

    inline void CheckInit()
    {
        if (isInit == false)
        {
            Init();
        }
    }

    String ReadFile(const String &fileName)
    {
        CheckInit();

        if (LITTLEFS.exists(fileName) == true)
        {
            File file = LITTLEFS.open(fileName, "r");
            auto data = file.readString();
            file.close();
            return data;
        }

        return String();
    }

    void WriteFile(const String &fileName, const String &data)
    {
        File file = LITTLEFS.open(fileName, FILE_WRITE);
        file.print(data.c_str());
        file.close();
    }

    const bool FileExists(const String &fileName)
    {
        CheckInit();
        return LITTLEFS.exists(fileName);
    }

    File OpenFile(const String &path, const char *mode)
    {
        CheckInit();
        return LITTLEFS.open(path, mode);
    }

    const bool DeleteFile(const String &path)
    {
        CheckInit();
        return LITTLEFS.remove(path);
    }

}