#pragma once

#include <atomic>
#include <functional>
#include <cstdint>
#include <cstddef>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class TcpServer {
public:
    using ClientStateCallback = std::function<void(bool isConnected)>;

    TcpServer();
    ~TcpServer();

    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    /**
     * @brief Creates the background TCP server listener task.
     * @param port Port number to listen on (default: 8080).
     * @return true if task was successfully spawned.
     */
    bool start(uint16_t port = 8080);

    /**
     * @brief Stops the server, closes client and server sockets, and deletes the listener task.
     */
    void stop();

    /**
     * @brief Checks if a active desktop client socket is connected.
     */
    bool isConnected() const;

    /**
     * @brief Transmits raw binary telemetry data over the active desktop socket.
     * @param data Pointer to binary buffer.
     * @param len Buffer byte length.
     * @return Number of bytes transmitted (0 if disconnected or write buffer full).
     */
    size_t sendData(const uint8_t* data, size_t len);

    /**
     * @brief Registers a callback to receive client connection and disconnect events.
     */
    void setOnClientStateChanged(ClientStateCallback callback);

private:
    uint16_t listen_port = 8080;
    std::atomic<int> server_fd{-1};
    std::atomic<int> client_fd{-1};

    TaskHandle_t task_handle = nullptr;
    ClientStateCallback client_state_callback = nullptr;

    static void serverTask(void* pvParameters);
    int createListeningSocket(uint16_t port);
    void handleClientSession(int active_sock);
    void drainClientSocket(int client_sock);
};
