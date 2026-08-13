#include "tasks/telemetry_task/telemetry_task.hpp"
#include "sensors/mpu6050/mpu6050.h"
#include "transport/transport_manager.hpp"
#include "app_config.h"
#include "freertos/task.h"
#include <esp_log.h>

namespace {
constexpr uint32_t kTelemetryTaskStackSize = 2048;
constexpr UBaseType_t kTelemetryTaskPriority = 2;

struct TelemetryTaskArgs {
    QueueHandle_t queue;
    TransportManager* transportManager;
};

void writeBigEndian16(uint8_t* dest, int16_t value) {
    dest[0] = static_cast<uint8_t>(static_cast<uint16_t>(value) >> 8);
    dest[1] = static_cast<uint8_t>(static_cast<uint16_t>(value) & 0xFF);
}

void telemetryTask(void* arg) {
    if (arg == nullptr) {
        ESP_LOGE("TelemetryTask", "Null task arguments provided");
        vTaskDelete(nullptr);
        return;
    }

    auto* args = static_cast<TelemetryTaskArgs*>(arg);
    QueueHandle_t queue = args->queue;
    TransportManager* transportManager = args->transportManager;

    if (queue == nullptr || transportManager == nullptr) {
        ESP_LOGE("TelemetryTask", "Invalid queue or transportManager argument");
        vTaskDelete(nullptr);
        return;
    }

    mpu6050_reading_t reading;

    while (true) {
        if (xQueueReceive(queue, &reading, portMAX_DELAY) == pdTRUE) {
            // Layout: [0xAA][0x55][device family id][accel_x][accel_y][accel_z]
            uint8_t packet[3 + sizeof(reading)];
            packet[0] = 0xAA;
            packet[1] = 0x55;
            packet[2] = DEVICE_FAMILY_ID;
            writeBigEndian16(&packet[3], reading.accel_x);
            writeBigEndian16(&packet[5], reading.accel_y);
            writeBigEndian16(&packet[7], reading.accel_z);

            transportManager->sendTelemetry(packet, sizeof(packet));
        }
    }
}
}

void telemetry_task_start(QueueHandle_t queue, TransportManager& transportManager) {
    if (queue == nullptr) {
        ESP_LOGE("TelemetryTask", "telemetry_task_start called with null queue");
        return;
    }
    static TelemetryTaskArgs args;
    args.queue = queue;
    args.transportManager = &transportManager;
    xTaskCreate(telemetryTask, "telemetry_task", kTelemetryTaskStackSize, &args, kTelemetryTaskPriority, nullptr);
}
