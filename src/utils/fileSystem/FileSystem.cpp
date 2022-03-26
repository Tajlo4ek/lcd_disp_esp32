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

        MUTEX_TASK(fsMutex,
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
        MUTEX_TASK(fsMutex,
                  {
                      File file = LITTLEFS.open(fileName, FILE_WRITE);
                      file.print(data.c_str());
                      file.close();
                  });
    }

    const bool FileExists(const String &fileName)
    {
        bool exist;

        MUTEX_TASK(fsMutex,
                  {
                      exist = LITTLEFS.exists(fileName);
                  });

        return exist;
    }

    File OpenFile(const String &path, const char *mode)
    {
        File file;

        MUTEX_TASK(fsMutex,
                  {
                      file = LITTLEFS.open(path, mode);
                  });

        return file;
    }

    const bool DeleteFile(const String &path)
    {
        bool deleted;

        MUTEX_TASK(fsMutex,
                  {
                      deleted = LITTLEFS.remove(path);
                  });

        return deleted;
    }

}