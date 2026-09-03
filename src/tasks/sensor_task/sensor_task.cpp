#include "tasks/sensor_task/sensor_task.hpp"
#include "sensors/motion_sensor.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <vector>

namespace {
constexpr uint32_t kSensorReadIntervalMs = 10;
constexpr uint32_t kSensorTaskStackSize = 2048;
constexpr UBaseType_t kSensorTaskPriority = 2;

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

    while (true) {
        if (motion_sensor_read_accel(&reading)) {
            for (QueueHandle_t queue : args->queues) {
                xQueueSend(queue, &reading, 0);
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
