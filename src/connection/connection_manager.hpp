#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "connection/transport/i_transport.hpp"

class ConnectionManager {
public:
    ConnectionManager();
    ~ConnectionManager();

    void init(ITransport* transport);
    void sendTelemetry(const uint8_t* data, size_t len);

private:
    ITransport* activeTransport = nullptr;
};

#endif