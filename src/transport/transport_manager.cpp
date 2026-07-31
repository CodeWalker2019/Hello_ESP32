#include "transport/transport_manager.hpp"
#include <esp_log.h>

TransportManager::TransportManager() {}

TransportManager::~TransportManager() {}

void TransportManager::setActiveTransport(ITransport* transport) {
    activeTransport = transport;
}

void TransportManager::resetActiveTransport() {
    activeTransport = nullptr;
}

void TransportManager::sendTelemetry(const uint8_t* data, size_t len) {
    if (activeTransport == nullptr) {
      ESP_LOGE("TransportManager", "send telemtry failed; activeTransport is not assigned");
      return;
    }

    activeTransport -> send(data, len);
}
