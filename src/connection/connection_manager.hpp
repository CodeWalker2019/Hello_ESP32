#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "transport/i_transport.hpp"
#include <array>
#include <vector>
#include <functional>
#include <mutex>

constexpr size_t kTransportCount = 2;

class ConnectionManager {
public:
    using TransportChangeCallback = std::function<void(ITransport*)>;

    explicit ConnectionManager(std::array<ITransport*, kTransportCount> transports);

    void init();
    void addOnTransportChangeListener(TransportChangeCallback callback);

private:
    std::mutex mutex_;
    ITransport* activeTransport = nullptr;
    std::array<ITransport*, kTransportCount> transportsList{};
    std::vector<TransportChangeCallback> onTransportChangeListeners;

    void handleTransportStateChange(ITransport* transport, bool isReady);
    void evaluateActiveTransport();
    ITransport* selectReadyTransport();
    void notifyListeners(ITransport* transport);
};

#endif