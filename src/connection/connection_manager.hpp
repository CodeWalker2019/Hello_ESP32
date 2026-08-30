#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include "transport/i_transport.hpp"
#include <vector>
#include <functional>
#include <mutex>

class ConnectionManager {
public:
    using TransportChangeCallback = std::function<void(ITransport*)>;
    explicit ConnectionManager(std::vector<ITransport*> transports);

    void init();
    void update();
    void addOnTransportChangeListener(TransportChangeCallback callback);

private:
    std::mutex mutex_;
    ITransport* activeTransport = nullptr;
    std::vector<ITransport*> transportsList;
    std::vector<TransportChangeCallback> onTransportChangeListeners;

    void evaluateActiveTransport();
    ITransport* selectReadyTransport();
    void notifyListeners(ITransport* transport);
};

#endif // CONNECTION_MANAGER_HPP