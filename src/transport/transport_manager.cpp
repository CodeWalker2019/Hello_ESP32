#include "transport/transport_manager.hpp"
#include <esp_log.h>

TransportManager::TransportManager() {}

TransportManager::~TransportManager() {}

void TransportManager::setActiveTransport(ITransport* transport) {
    activeTransport.store(transport);
}

void TransportManager::resetActiveTransport() {
    activeTransport.store(nullptr);
}

void TransportManager::sendTelemetry(const uint8_t* data, size_t len) {
    ITransport* transport = activeTransport.load();
    if (transport != nullptr) transport->send(data, len);
}
