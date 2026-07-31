#ifndef TRANSPORT_MANAGER_HPP
#define TRANSPORT_MANAGER_HPP

#include <array>
#include "transport/i_transport.hpp"

class TransportManager {
public:
    TransportManager();
    ~TransportManager();

    void resetActiveTransport();
    void setActiveTransport(ITransport* transport);
    void sendTelemetry(const uint8_t* data, size_t len);

private:
    ITransport* activeTransport = nullptr;
};

#endif // TRANSPORT_MANAGER_HPP
