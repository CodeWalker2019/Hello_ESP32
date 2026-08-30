#include "app_config.h"
#include "system_orchestrator.hpp"
#include "led_status/led_status.hpp"
#include "connection/connection_manager.hpp"
#include "transport/transport_manager.hpp"
#include "transport/usb_transport/usb_transport.hpp"

#define CONN_MGR_TASK_NAME              "conn_mgr_task"
#define CONN_MGR_TASK_STACK_BYTES       3072
#define CONN_MGR_TASK_PRIORITY          5
#define CONN_MGR_POLL_INTERVAL_MS       500

static UsbTransport usbTransport;
static TransportManager transportManager;
static ConnectionManager connectionManager({ &usbTransport });

extern "C" esp_err_t system_orchestrator_worker(void) {
    QueueHandle_t sensorQueue = sensor_task_start();
    if (sensorQueue != nullptr) {
        telemetry_task_start(sensorQueue, transportManager);
    } else {
        ESP_LOGE("AppBridge", "Cannot start telemetry task, sensorQueue is null");
        return ESP_FAIL;
    }

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
