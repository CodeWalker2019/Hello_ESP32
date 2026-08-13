#include "app_config.h"
#include "app_bridge.hpp"
#include "sensors/mpu6050/mpu6050.h"
#include "led_status/led_status.hpp"
#include "connection/connection_manager.hpp"
#include "transport/transport_manager.hpp"
#include "transport/usb_transport/usb_transport.hpp"
#include "tasks/sensor_task/sensor_task.h"
#include "tasks/telemetry_task/telemetry_task.hpp"
#include <esp_log.h>

static UsbTransport usbTransport;
static TransportManager transportManager;
static ConnectionManager connectionManager({&usbTransport, nullptr});

extern "C" void app_bridge_init(void) {
    led_status_init();
    connectionManager.init();

    mpu6050_init_motion_sensor();
    mpu6050_check_present();

    QueueHandle_t sensorQueue = sensor_task_start();

    if (sensorQueue != nullptr) telemetry_task_start(sensorQueue, transportManager);
    else ESP_LOGE("AppBridge", "Cannot start telemetry task, sensorQueue is null");

    connectionManager.addOnTransportChangeListener([](ITransport* transport) {
        led_status_set(transport != nullptr ? LED_STATE_CONNECTED : LED_STATE_SEARCH);
    });
}

extern "C" void app_bridge_connection_manager_worker(void) {
    connectionManager.update();
}
