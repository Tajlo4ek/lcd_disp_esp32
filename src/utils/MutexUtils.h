#ifndef _MUTEX_UTILS_H_
#define _MUTEX_UTILS_H_

#define MUTEX_TASK(mutex, task) {xSemaphoreTake(mutex, portMAX_DELAY); task; xSemaphoreGive(mutex);}

#endif