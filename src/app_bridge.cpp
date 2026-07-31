#include "app_config.h"
#include "app_bridge.hpp"
#include "led_status/led_status.hpp"
#include "connection/connection_manager.hpp"
#include "connection/transport/usb_transport/usb_transport.hpp"

static UsbTransport usbTransport;
static ConnectionManager connectionManager;

extern "C" void app_bridge_init(void) {
    led_status_init();
    connectionManager.init(&usbTransport);
}

extern "C" void app_bridge_send_test_data(void) {
    uint8_t packet[3] = { 0xAA, 0x55, DEVICE_FAMILY_ID };
    connectionManager.sendTelemetry(packet, sizeof(packet));
}
