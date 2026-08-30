#include "tasks/sensor_task/sensor_task.h"
#include "sensors/motion_sensor.h"
#include "freertos/task.h"
#include <esp_log.h>

namespace {
    constexpr uint32_t kSensorReadIntervalMs = 10;
    constexpr uint32_t kSensorTaskStackSize = 2048;
    constexpr UBaseType_t kSensorTaskPriority = 2;

    void sensorTask(void* arg) {
        if (arg == nullptr) {
            ESP_LOGE("SensorTask", "Null queue passed to sensor task");
            vTaskDelete(nullptr);
            return;
        }

        QueueHandle_t queue = static_cast<QueueHandle_t>(arg);
        motion_sensor_reading_t reading;

        while (true) {
            if (motion_sensor_read_accel(&reading)) {
                xQueueSend(queue, &reading, 0);
            }
            vTaskDelay(pdMS_TO_TICKS(kSensorReadIntervalMs));
        }
    }
}

esp_err_t sensor_task_start(QueueHandle_t queue) {
    if (queue == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    BaseType_t res = xTaskCreate(sensorTask, "sensor_task", kSensorTaskStackSize, queue, kSensorTaskPriority, nullptr);
    return (res == pdPASS) ? ESP_OK : ESP_FAIL;
}

