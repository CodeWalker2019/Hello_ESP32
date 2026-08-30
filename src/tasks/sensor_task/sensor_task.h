#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

esp_err_t sensor_task_start(QueueHandle_t queue);

#endif

