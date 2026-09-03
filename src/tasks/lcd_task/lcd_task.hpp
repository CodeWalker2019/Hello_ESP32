#pragma once

#include "esp_err.h"
#include "freertos/queue.h"

esp_err_t lcd_task_start(QueueHandle_t queue, const char* taskName = "lcd_task");
