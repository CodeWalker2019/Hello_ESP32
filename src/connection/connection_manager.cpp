#include "connection/connection_manager.hpp"
#include <esp_log.h>

static const char* TAG = "ConnectionManager";

ConnectionManager::ConnectionManager(std::array<ITransport*, kTransportCount> transports)
    : transportsList(transports) {}

void ConnectionManager::init() {
    for (ITransport* transport : transportsList) {
        if (transport != nullptr) {
            transport->init();
        }
    }
}

void ConnectionManager::addOnTransportChangeListener(TransportChangeCallback callback) {
    onTransportChangeListeners.push_back(callback);
}

void ConnectionManager::notifyListeners(ITransport* transport) {
    for (auto& listener : onTransportChangeListeners) {
        listener(transport);
    }
}

void ConnectionManager::update() {
    ITransport* readyTransport = listenReadyTransport();

    if (readyTransport != activeTransport) {
        activeTransport = readyTransport;
        notifyListeners(activeTransport);
    }
}

ITransport* ConnectionManager::listenReadyTransport() {
    if (activeTransport != nullptr && activeTransport->isReady()) {
        return activeTransport;
    }

    for (ITransport* transport : transportsList) {
        if (transport != nullptr && transport->isReady()) {
            return transport;
        }
    }

    return nullptr;
}
