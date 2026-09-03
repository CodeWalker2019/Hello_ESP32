#pragma once

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <initializer_list>

esp_err_t sensor_task_start(std::initializer_list<QueueHandle_t> queues);
