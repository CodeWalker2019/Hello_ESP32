#include "tasks/sensor_task/sensor_task.h"
#include "sensors/motion_sensor.h"
#include "freertos/task.h"
#include <esp_log.h>

namespace {
    constexpr uint32_t kSensorReadIntervalMs = 10;
    constexpr uint32_t kSensorTaskStackSize = 2048;
    constexpr UBaseType_t kSensorTaskPriority = 2;
    constexpr UBaseType_t kQueueLength = 10;

    QueueHandle_t sensor_queue = nullptr;

    void sensorTask(void* arg) {
        motion_sensor_reading_t reading;

        while (true) {
            if (motion_sensor_read_accel(&reading)) {
                xQueueSend(sensor_queue, &reading, 0);
            }
            vTaskDelay(pdMS_TO_TICKS(kSensorReadIntervalMs));
        }
    }
}

QueueHandle_t sensor_task_start(void) {
    sensor_queue = xQueueCreate(kQueueLength, sizeof(motion_sensor_reading_t));
    if (sensor_queue == nullptr) {
        ESP_LOGE("SensorTask", "Failed to create sensor queue");
        return nullptr;
    }
    xTaskCreate(sensorTask, "sensor_task", kSensorTaskStackSize, nullptr, kSensorTaskPriority, nullptr);
    return sensor_queue;
}
