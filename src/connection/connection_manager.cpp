#include "connection/connection_manager.hpp"
#include <esp_log.h>

static const char* TAG = "ConnectionManager";

ConnectionManager::ConnectionManager(std::vector<ITransport*> transports)
    : transportsList(std::move(transports)) {}

void ConnectionManager::init() {
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

void ConnectionManager::handleTransportStateChange(ITransport* transport, bool isReady) {
    ITransport* newlySelectedTransport = nullptr;
    bool stateChanged = false;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        ESP_LOGI(TAG, "Transport state changed (isReady: %d)", isReady);
        
        ITransport* readyTransport = selectReadyTransport();
        if (readyTransport != activeTransport) {
            activeTransport = readyTransport;
            newlySelectedTransport = activeTransport;
            stateChanged = true;
        }

        for (ITransport* t : transportsList) {
            if (t != nullptr) {
                t->setBeaconEnabled(t != activeTransport);
            }
        }
    }

    if (stateChanged) {
        notifyListeners(newlySelectedTransport);
    }
}

void ConnectionManager::evaluateActiveTransport() {
    ITransport* newlySelectedTransport = nullptr;
    bool stateChanged = false;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        ITransport* readyTransport = selectReadyTransport();

        if (readyTransport != activeTransport) {
            activeTransport = readyTransport;
            newlySelectedTransport = activeTransport;
            stateChanged = true;
        }

        for (ITransport* transport : transportsList) {
            if (transport != nullptr) {
                transport->setBeaconEnabled(transport != activeTransport);
            }
        }
    }

    if (stateChanged) {
        notifyListeners(newlySelectedTransport);
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

void ConnectionManager::notifyListeners(ITransport* transport) {
    std::vector<TransportChangeCallback> callbacks_copy;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        callbacks_copy = onTransportChangeListeners;
    }

    for (const auto& listener : callbacks_copy) {
        if (listener) {
            listener(transport);
        }
    }
}
