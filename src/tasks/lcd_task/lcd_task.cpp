#include "freertos/FreeRTOS.h"
#include "tasks/lcd_task/lcd_task.hpp"
#include "tasks/consumer_task.hpp"
#include "sensors/motion_sensor.h"

esp_err_t lcd_task_start(QueueHandle_t queue, const char* taskName) {
    return consumer_task_start<motion_sensor_reading_t>(
        queue,
        [](const motion_sensor_reading_t& reading) {
            // lcd_display_show_reading(reading);
        },
        taskName);
}
