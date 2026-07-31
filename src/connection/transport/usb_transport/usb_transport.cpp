#include "connection/transport/usb_transport/usb_transport.hpp"
#include "app_config.h"

UsbTransport::UsbTransport() {}

UsbTransport::~UsbTransport() {
#if !TELEMETRY_MODE_TEXT_DEBUG
    uart_driver_delete(UART_PORT);
#endif
}

bool UsbTransport::init() {
#if TELEMETRY_MODE_TEXT_DEBUG // In Dev mode UART driver is enabled by default
    initialized = true;
    return true;
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
    return true;
#endif
}

size_t UsbTransport::send(const uint8_t* data, size_t len) {
#if TELEMETRY_MODE_TEXT_DEBUG
    printf("%.*s", (int)len, (const char*)data);
    return len;
#else
    const int write_result = uart_write_bytes(UART_PORT, (const char*)data, len);

    if (write_result < 0) {
        ESP_LOGE("UsbTransport", "uart_write_bytes failed");
        return 0;
    }

    return (size_t)write_result;
#endif
}
