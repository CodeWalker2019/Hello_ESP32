#include "freertos/FreeRTOS.h"
#include "tasks/lcd_task/lcd_task.hpp"
#include "tasks/consumer_task.hpp"
#include "sensors/orientation/orientation_filter.h"
#include "display_driver/hd44780.h"
#include <cstdio>

esp_err_t lcd_task_start(QueueHandle_t queue, const char* taskName) {
    return consumer_task_start<motion_orientation_t>(
        queue,
        [](const motion_orientation_t& orientation) {
            char roll_line[17];
            char pitch_line[17];
            snprintf(roll_line, sizeof(roll_line), "Roll: %6.1f", orientation.roll);
            snprintf(pitch_line, sizeof(pitch_line), "Pitch:%6.1f", orientation.pitch);

            char display[33];
            snprintf(display, sizeof(display), "%-16s%-16s", roll_line, pitch_line);
            lcd_write_str32(display);
        },
        taskName);
}
