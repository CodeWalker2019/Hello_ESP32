#pragma once

#include "transport/i_transport.hpp"
#include "transport/wifi_transport/wifi_netif_manager/wifi_netif_manager.hpp"
#include "transport/wifi_transport/wifi_touch_handler/wifi_touch_handler.hpp"
#include "transport/wifi_transport/tcp_server_task/tcp_server_task.hpp"

class WifiTransport : public ITransport {
public:
    WifiTransport();
    ~WifiTransport() override;

    WifiTransport(const WifiTransport&) = delete;
    WifiTransport& operator=(const WifiTransport&) = delete;

    /**
     * @brief Initializes subcomponents (NetifManager, TouchHandler, TcpServer) 
     *        and starts APSTA coexistence mode.
     */
    bool init() override;

    /**
     * @brief Checks if an active desktop TCP client socket is open and ready for telemetry.
     */
    bool isReady() const override;

    /**
     * @brief Enables or disables the SoftAP beacon visibility.
     */
    void setBeaconEnabled(bool enabled) override;

    /**
     * @brief Transmits raw binary telemetry data over the active TCP server socket.
     */
    size_t send(const uint8_t* data, size_t len) override;

    /**
     * @brief Registers state change listener for overall transport readiness.
     */
    void setOnStateChangeListener(StateChangeCallback callback) override;

private:
    WifiNetifManager netifManager;
    WifiTouch touchHandler;
    TcpServer tcpServer;

    StateChangeCallback stateChangeCallback = nullptr;

    void handleCredentialsReceived(const wifi_config_t& wifi_cfg);
    void handleConnectionStatusChanged(bool isConnected);
    void handleClientStateChanged(bool isConnected);
};
