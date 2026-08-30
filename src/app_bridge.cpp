#include "app_config.h"
#include "app_bridge.hpp"
#include "led_status/led_status.hpp"
#include "connection/connection_manager.hpp"
#include "transport/transport_manager.hpp"
#include "transport/usb_transport/usb_transport.hpp"
#include "sensors/motion_sensor.h"
#include "tasks/sensor_task/sensor_task.h"
#include "tasks/telemetry_task/telemetry_task.hpp"
#include <esp_log.h>

static UsbTransport usbTransport;
static TransportManager transportManager;
static ConnectionManager connectionManager({&usbTransport, nullptr});

extern "C" esp_err_t system_orchestrator_init(void) {
    led_status_init();

    motion_sensor_init();
    motion_sensor_check_present();

    QueueHandle_t sensorQueue = sensor_task_start();

    if (sensorQueue != nullptr) telemetry_task_start(sensorQueue, transportManager);
    else ESP_LOGE("AppBridge", "Cannot start telemetry task, sensorQueue is null");

    connectionManager.addOnTransportChangeListener([](ITransport* transport) {
        if (transport != nullptr) {
            transportManager.setActiveTransport(transport);
            return;
        } 

        transportManager.resetActiveTransport();
    });

    connectionManager.addOnTransportChangeListener([](ITransport* transport) {
        led_status_set(transport != nullptr ? LED_STATE_CONNECTED : LED_STATE_SEARCH);
    });

    return ESP_OK;
}
