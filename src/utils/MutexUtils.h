#ifndef _MUTEX_UTILS_H_
#define _MUTEX_UTILS_H_

#define MutexTask(mutex, task) {xSemaphoreTake(mutex, portMAX_DELAY); task; xSemaphoreGive(mutex);}

#endif