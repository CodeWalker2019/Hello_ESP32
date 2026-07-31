#include "app_config.h"
#include "app_bridge.hpp"
#include "led_status/led_status.hpp"
#include "connection/connection_manager.hpp"
#include "transport/transport_manager.hpp"
#include "transport/usb_transport/usb_transport.hpp"

static UsbTransport usbTransport;
static TransportManager transportManager;
static ConnectionManager connectionManager({&usbTransport, nullptr}); // nullptr — WifiTransport ще не готовий

extern "C" void app_bridge_init(void) {
    led_status_init();
    connectionManager.init();

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
}

extern "C" void app_bridge_connection_manager_worker(void) {
    connectionManager.update();
}
