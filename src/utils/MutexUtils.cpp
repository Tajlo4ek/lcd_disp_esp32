#include "MutexUtils.h"

namespace MutexUtils
{
    void MutexTask(const SemaphoreHandle_t &mutex, const std::function<void()> task)
    {
        xSemaphoreTake(mutex, portMAX_DELAY);
        task();
        xSemaphoreGive(mutex);
    }
}
