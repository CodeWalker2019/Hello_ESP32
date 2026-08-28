#include "connection/connection_manager.hpp"
#include <esp_log.h>

static const char* TAG = "ConnectionManager";

ConnectionManager::ConnectionManager(std::array<ITransport*, kTransportCount> transports)
    : transportsList(transports) {}

void ConnectionManager::init() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (ITransport* transport : transportsList) {
        if (transport != nullptr) {
            transport->setOnStateChangeListener([this](ITransport* t, bool ready) {
                this->handleTransportStateChange(t, ready);
            });
            transport->init();
        }
    }
    evaluateActiveTransport();
}

void ConnectionManager::addOnTransportChangeListener(TransportChangeCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    onTransportChangeListeners.push_back(callback);
}

void ConnectionManager::notifyListeners(ITransport* transport) {
    for (auto& listener : onTransportChangeListeners) {
        listener(transport);
    }
}

void ConnectionManager::handleTransportStateChange(ITransport* transport, bool isReady) {
    std::lock_guard<std::mutex> lock(mutex_);
    ESP_LOGI(TAG, "Transport state changed (isReady: %d)", isReady);
    evaluateActiveTransport();
}

void ConnectionManager::evaluateActiveTransport() {
    ITransport* readyTransport = selectReadyTransport();

    if (readyTransport != activeTransport) {
        activeTransport = readyTransport;
        notifyListeners(activeTransport);
    }

    for (ITransport* transport : transportsList) {
        if (transport != nullptr) {
            transport->setBeaconEnabled(transport != activeTransport);
        }
    }
}

ITransport* ConnectionManager::selectReadyTransport() {
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

