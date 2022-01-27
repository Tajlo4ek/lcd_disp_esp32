#ifndef _MUTEX_UTILS_H_
#define _MUTEX_UTILS_H_

#include <functional>
#include <Arduino.h>

namespace MutexUtils
{
    void MutexTask(const SemaphoreHandle_t &mutex, const std::function<void()> task);
}

#endif