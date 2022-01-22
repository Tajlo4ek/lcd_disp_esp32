#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include <Arduino.h>
#include <LITTLEFS.h>


namespace FileSystem
{
    void Init();

    String ReadFile(const String &fileName);
    void WriteFile(const String &fileName, const String &data);

    const bool FileExists(const String &fileName);

    File OpenFile(const String &path, const char *mode);

    const bool DeleteFile(const String &path);
}

#endif