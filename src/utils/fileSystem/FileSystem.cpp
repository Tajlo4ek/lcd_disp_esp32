#include "FileSystem.h"

#include "utils/FileNames.h"
#include "utils/MutexUtils.h"

#define CONFIG_LITTLEFS_SPIFFS_COMPAT 1

namespace FileSystem
{
    SemaphoreHandle_t fsMutex = xSemaphoreCreateMutex();

    void Init()
    {
        fsMutex = xSemaphoreCreateMutex();

        LITTLEFS.begin();

        LITTLEFS.mkdir(CONFIG_DIR);
        LITTLEFS.mkdir(PAGES_DIR);
    }

    String ReadFile(const String &fileName)
    {
        String data;

        MutexUtils::MutexTask(fsMutex, [&fileName, &data]() mutable
                              {
                                  if (LITTLEFS.exists(fileName) == true)
                                  {
                                      File file = LITTLEFS.open(fileName, "r");
                                      data = file.readString();
                                      file.close();
                                  }
                              });

        return data;
    }

    void WriteFile(const String &fileName, const String &data)
    {
        MutexUtils::MutexTask(fsMutex, [&fileName, &data]() mutable
                              {
                                  File file = LITTLEFS.open(fileName, FILE_WRITE);
                                  file.print(data.c_str());
                                  file.close();
                              });
    }

    const bool FileExists(const String &fileName)
    {
        bool exist;

        MutexUtils::MutexTask(fsMutex, [&fileName, &exist]() mutable
                              { exist = LITTLEFS.exists(fileName); });

        return exist;
    }

    File OpenFile(const String &path, const char *mode)
    {
        File file;

        MutexUtils::MutexTask(fsMutex, [&path, &mode, &file]() mutable
                              { file = LITTLEFS.open(path, mode); });

        return file;
    }

    const bool DeleteFile(const String &path)
    {
        bool deleted;

        MutexUtils::MutexTask(fsMutex, [&path, &deleted]() mutable
                              { deleted = LITTLEFS.remove(path); });

        return deleted;
    }

}