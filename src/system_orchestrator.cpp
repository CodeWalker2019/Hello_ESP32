#include "app_config.h"
#include "system_orchestrator.hpp"
#include "led_status/led_status.hpp"
#include "sensors/motion_sensor.h"
#include "tasks/sensor_task/sensor_task.h"
#include "tasks/telemetry_task/telemetry_task.hpp"
#include "connection/connection_manager.hpp"
#include "transport/transport_manager.hpp"
#include "transport/usb_transport/usb_transport.hpp"
#include "esp_check.h"
#include "esp_log.h"

#define SENSOR_QUEUE_LENGTH 10

static UsbTransport usbTransport;
static TransportManager transportManager;
static ConnectionManager connectionManager({ &usbTransport });

extern "C" esp_err_t system_orchestrator_init(void) {
    QueueHandle_t sensorQueue = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(motion_sensor_reading_t));
    if (sensorQueue == nullptr) {
        return ESP_ERR_NO_MEM;
    }

    ESP_RETURN_ON_ERROR(sensor_task_start(sensorQueue), SYSTEM_ORCHESTRATOR_LOGS, "Failed to start sensor task");
    ESP_RETURN_ON_ERROR(telemetry_task_start(sensorQueue, transportManager), SYSTEM_ORCHESTRATOR_LOGS, "Failed to start telemetry task");

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

