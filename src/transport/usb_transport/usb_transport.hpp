#ifndef USB_TRANSPORT_HPP
#define USB_TRANSPORT_HPP

#include "transport/i_transport.hpp"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class UsbTransport : public ITransport {
public:
    UsbTransport();
    ~UsbTransport() override;

    bool init() override;
    bool isReady() const override;
    size_t send(const uint8_t* data, size_t len) override;

private:
    static constexpr uart_port_t UART_PORT = UART_NUM_0;
    bool initialized = false;
    volatile bool heartbeat_alive = false;
    TaskHandle_t listener_task_handle = nullptr;

    static void heartbeatListenerTask(void* arg);
};

#endif
