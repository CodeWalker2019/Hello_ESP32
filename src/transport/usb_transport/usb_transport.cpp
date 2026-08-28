#include "transport/usb_transport/usb_transport.hpp"
#include "transport/heartbeat/heartbeat_matcher.hpp"
#include "app_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <cstdio>

namespace {

constexpr uint32_t kHeartbeatTimeoutMs = 5000;
constexpr uint32_t kListenerTaskStackSize = 2048;
constexpr UBaseType_t kListenerTaskPriority = 1;
constexpr uint8_t kHeartbeatMagic[3] = {0xAA, 0x55, 0x02};
constexpr uint32_t kBeaconIntervalMs = 1000;
constexpr uint8_t kIdentityBeacon[3] = {0xAA, 0x55, DEVICE_FAMILY_ID};

bool readIncomingByte(uart_port_t port, uint8_t* outByte) {
#if TELEMETRY_MODE_TEXT_DEBUG
    int c = getchar();
    if (c == EOF) return false;
    *outByte = static_cast<uint8_t>(c);
    return true;
#else
    int len = uart_read_bytes(port, outByte, 1, pdMS_TO_TICKS(100));
    return len > 0;
#endif
}

}  // namespace

UsbTransport::UsbTransport() {}

UsbTransport::~UsbTransport() {
    if (listener_task_handle != nullptr) {
        vTaskDelete(listener_task_handle);
    }
#if !TELEMETRY_MODE_TEXT_DEBUG
    uart_driver_delete(UART_PORT);
#endif
}

bool UsbTransport::init() {
#if TELEMETRY_MODE_TEXT_DEBUG
    initialized = true;
#else
    uart_config_t config = {
        .baud_rate = BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    if (uart_param_config(UART_PORT, &config) != ESP_OK) return false;
    if (uart_driver_install(UART_PORT, 1024, 1024, 0, NULL, 0) != ESP_OK) return false;
    initialized = true;
#endif
    xTaskCreate(heartbeatListenerTask, "usb_heartbeat", kListenerTaskStackSize, this,
                kListenerTaskPriority, &listener_task_handle);
    return initialized;
}

void UsbTransport::setOnStateChangeListener(StateChangeCallback callback) {
    state_change_callback = std::move(callback);
}

void UsbTransport::notifyStateChange(bool ready) {
    if (state_change_callback) {
        state_change_callback(this, ready);
    }
}

bool UsbTransport::isReady() const {
    return heartbeat_alive.load();
}

void UsbTransport::setBeaconEnabled(bool enabled) {
    beacon_enabled.store(enabled);
}

size_t UsbTransport::send(const uint8_t* data, size_t len) {
#if TELEMETRY_MODE_TEXT_DEBUG
    printf("%.*s", (int)len, (const char*)data);
    return len;
#else
    int written = uart_write_bytes(UART_PORT, (const char*)data, len);
    if (written < 0) {
        ESP_LOGE("UsbTransport", "uart_write_bytes failed");
        return 0;
    }
    return (size_t)written;
#endif
}

void UsbTransport::heartbeatListenerTask(void* arg) {
    UsbTransport* self = static_cast<UsbTransport*>(arg);
    HeartbeatMatcher matcher(kHeartbeatMagic, sizeof(kHeartbeatMagic));
    TickType_t lastSeen = xTaskGetTickCount();
    TickType_t lastBeaconSent = xTaskGetTickCount() - pdMS_TO_TICKS(kBeaconIntervalMs);

    while (true) {
        uint8_t byte;
        if (readIncomingByte(UART_PORT, &byte) && matcher.feed(byte)) {
            lastSeen = xTaskGetTickCount();
            if (!self->heartbeat_alive.exchange(true)) {
                self->notifyStateChange(true);
            }
        }

        if (self->heartbeat_alive.load() &&
            (xTaskGetTickCount() - lastSeen) > pdMS_TO_TICKS(kHeartbeatTimeoutMs)) {
            self->heartbeat_alive.store(false);
            self->notifyStateChange(false);
        }

        if (self->beacon_enabled.load() &&
            (xTaskGetTickCount() - lastBeaconSent) >= pdMS_TO_TICKS(kBeaconIntervalMs)) {
            self->send(kIdentityBeacon, sizeof(kIdentityBeacon));
            lastBeaconSent = xTaskGetTickCount();
        }
    }
}
