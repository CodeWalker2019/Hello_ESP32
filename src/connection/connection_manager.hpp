#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "transport/i_transport.hpp"
#include <array>
#include <vector>
#include <functional>

constexpr size_t kTransportCount = 2;

class ConnectionManager {
public:
    using TransportChangeCallback = std::function<void(ITransport*)>;

    explicit ConnectionManager(std::array<ITransport*, kTransportCount> transports);

    void init();
    void addOnTransportChangeListener(TransportChangeCallback callback);
    void update();

private:
    ITransport* activeTransport = nullptr;
    std::array<ITransport*, kTransportCount> transportsList{};
    std::vector<TransportChangeCallback> onTransportChangeListeners;

    ITransport* listenReadyTransport();
    void notifyListeners(ITransport* transport);
};

#endif