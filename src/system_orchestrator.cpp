#include "app_config.h"
#include "system_orchestrator.hpp"
#include "led_status/led_status.hpp"
#include "sensors/orientation/orientation_filter.h"
#include "tasks/sensor_task/sensor_task.hpp"
#include "tasks/telemetry_task/telemetry_task.hpp"
#include "tasks/lcd_task/lcd_task.hpp"
#include "connection/connection_manager.hpp"
#include "transport/transport_manager.hpp"
#include "transport/usb_transport/usb_transport.hpp"
#include "transport/wifi_transport/wifi_transport.hpp"
#include "esp_check.h"
#include "esp_log.h"

#define SENSOR_QUEUE_LENGTH 10

static UsbTransport usbTransport;
static WifiTransport wifiTransport;
static TransportManager transportManager;
static ConnectionManager connectionManager({ &usbTransport, &wifiTransport });

extern "C" esp_err_t system_orchestrator_init(void) {
    QueueHandle_t telemetryQueue = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(motion_orientation_t));
    QueueHandle_t lcdQueue = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(motion_orientation_t));
    if (telemetryQueue == nullptr || lcdQueue == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    ESP_RETURN_ON_ERROR(sensor_task_start({ telemetryQueue, lcdQueue }),
                         SYSTEM_ORCHESTRATOR_LOGS, "Failed to start sensor task");

    ESP_RETURN_ON_ERROR(telemetry_task_start(telemetryQueue,
        [](const uint8_t* data, size_t len) {
            transportManager.sendTelemetry(data, len);
        }, "telemetry_task"),
        SYSTEM_ORCHESTRATOR_LOGS, "Failed to start telemetry task");

    ESP_RETURN_ON_ERROR(lcd_task_start(lcdQueue, "lcd_task"),
                         SYSTEM_ORCHESTRATOR_LOGS, "Failed to start lcd task");

    connectionManager.init();
    connectionManager.addOnTransportChangeListener([](ITransport* transport) {
        if (transport != nullptr) {
            transportManager.setActiveTransport(transport);
            led_status_set(LED_STATE_CONNECTED);
        } else {
            transportManager.resetActiveTransport();
            led_status_set(LED_STATE_SEARCH);
        }
    });

    return ESP_OK;
}
