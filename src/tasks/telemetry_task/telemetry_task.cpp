#include "freertos/FreeRTOS.h"
#include "tasks/telemetry_task/telemetry_task.hpp"
#include "tasks/consumer_task.hpp"
#include "sensors/orientation/orientation_filter.h"
#include "app_config.h"

namespace {
// Fixed-point scale for wire transmission: degrees * 100, e.g. 45.23 -> 4523.
// Keeps the existing int16 big-endian wire style instead of introducing raw
// float encoding; +/-180 deg comfortably fits int16 range at this scale.
constexpr float kDegreeScale = 100.0f;

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

    return consumer_task_start<motion_orientation_t>(
        queue,
        [sendTelemetry](const motion_orientation_t& orientation) {
            // Layout: [0xAA][0x55][device family id][roll*100][pitch*100], both int16 BE
            int16_t roll_scaled = static_cast<int16_t>(orientation.roll * kDegreeScale);
            int16_t pitch_scaled = static_cast<int16_t>(orientation.pitch * kDegreeScale);

            uint8_t packet[7];
            packet[0] = 0xAA;
            packet[1] = 0x55;
            packet[2] = DEVICE_FAMILY_ID;
            writeBigEndian16(&packet[3], roll_scaled);
            writeBigEndian16(&packet[5], pitch_scaled);
            sendTelemetry(packet, sizeof(packet));
        },
        taskName);
}
