#include "connection/connection_manager.hpp"
#include <esp_log.h>

ConnectionManager::ConnectionManager() {}

ConnectionManager::~ConnectionManager() {}

void ConnectionManager::init(ITransport* transport) {
    activeTransport = transport;
    const bool success_init = transport -> init();

    if (!success_init) {
      ESP_LOGE("ConnectionManager", "transport init failed");
    }
}

void ConnectionManager::sendTelemetry(const uint8_t* data, size_t len) {
    if (activeTransport == nullptr) {
      ESP_LOGE("ConnectionManager", "send telemtry failed; activeTransport is not assigned");
      return;
    }

    activeTransport -> send(data, len);
}
