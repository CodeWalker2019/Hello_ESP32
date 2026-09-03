#include "freertos/FreeRTOS.h" 
#include "tasks/telemetry_task/telemetry_task.hpp"
#include "tasks/consumer_task.hpp"
#include "sensors/motion_sensor.h"
#include "app_config.h"

namespace {
void writeBigEndian16(uint8_t* dest, int16_t value) {
    dest[0] = static_cast<uint8_t>(static_cast<uint16_t>(value) >> 8);
    dest[1] = static_cast<uint8_t>(static_cast<uint16_t>(value) & 0xFF);
}
}

esp_err_t telemetry_task_start(QueueHandle_t queue, SendTelemetryFn sendTelemetry, const char* taskName) {
    if (!sendTelemetry) {
        ESP_LOGE("TelemetryTask", "telemetry_task_start called with null sendTelemetry");
        return ESP_ERR_INVALID_ARG;
    }

    return consumer_task_start<motion_sensor_reading_t>(
        queue,
        [sendTelemetry](const motion_sensor_reading_t& reading) {
            // Layout: [0xAA][0x55][device family id][accel_x][accel_y][accel_z]
            uint8_t packet[3 + sizeof(reading)];
            packet[0] = 0xAA;
            packet[1] = 0x55;
            packet[2] = DEVICE_FAMILY_ID;
            writeBigEndian16(&packet[3], reading.accel_x);
            writeBigEndian16(&packet[5], reading.accel_y);
            writeBigEndian16(&packet[7], reading.accel_z);
            sendTelemetry(packet, sizeof(packet));
        },
        taskName);
}
