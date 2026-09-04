#include "tasks/sensor_task/sensor_task.hpp"
#include "sensors/motion_sensor.h"
#include "sensors/orientation/orientation_filter.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <esp_log.h>
#include <vector>

namespace {
constexpr uint32_t kSensorReadIntervalMs = 10;
constexpr uint32_t kSensorTaskStackSize = 2048;
constexpr UBaseType_t kSensorTaskPriority = 2;
constexpr float kMicrosecondsPerSecond = 1e6f;

struct SensorTaskArgs {
    std::vector<QueueHandle_t> queues;
};

void sensorTask(void* arg) {
    if (arg == nullptr) {
        ESP_LOGE("SensorTask", "Null args passed to sensor task");
        vTaskDelete(nullptr);
        return;
    }

    auto* args = static_cast<SensorTaskArgs*>(arg);
    motion_sensor_reading_t reading;
    // Sole caller of orientation_filter_update(): its fused state is not
    // thread-safe, so only this task may drive it.
    int64_t last_read_us = 0;

    while (true) {
        if (motion_sensor_read(&reading)) {
            int64_t now_us = esp_timer_get_time();
            float dt_seconds = (last_read_us == 0) ? 0.0f : (float)(now_us - last_read_us) / kMicrosecondsPerSecond;
            last_read_us = now_us;

            motion_orientation_t orientation = orientation_filter_update(
                reading.accel_x, reading.accel_y, reading.accel_z,
                reading.gyro_x, reading.gyro_y,
                dt_seconds);

            for (QueueHandle_t queue : args->queues) {
                xQueueSend(queue, &orientation, 0);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(kSensorReadIntervalMs));
    }
}
}

esp_err_t sensor_task_start(std::initializer_list<QueueHandle_t> queues) {
    if (queues.size() == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    for (QueueHandle_t q : queues) {
        if (q == nullptr) {
            return ESP_ERR_INVALID_ARG;
        }
    }

    auto* args = new (std::nothrow) SensorTaskArgs{queues};
    if (args == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    BaseType_t res = xTaskCreate(sensorTask, "sensor_task", kSensorTaskStackSize, args, kSensorTaskPriority, nullptr);
    if (res != pdPASS) {
        delete args;
        return ESP_FAIL;
    }
    return ESP_OK;
}
