#ifndef USB_TRANSPORT_HPP
#define USB_TRANSPORT_HPP

#include "connection/transport/i_transport.hpp"
#include "driver/uart.h"

class UsbTransport : public ITransport {
public:
    UsbTransport();
    ~UsbTransport() override;

    bool init() override;
    size_t send(const uint8_t* data, size_t len) override;

private:
    static constexpr uart_port_t UART_PORT = UART_NUM_0;
    bool initialized = false;
};

#endif
