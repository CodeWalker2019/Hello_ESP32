#include "transport/wifi_transport/tcp_server_task/tcp_server_task.hpp"
#include <esp_log.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <cerrno>

static const char* TAG = "TcpServer";
static constexpr uint32_t TCP_SERVER_STACK_DEPTH = 4096;
static constexpr UBaseType_t TCP_SERVER_TASK_PRIORITY = 5;
static constexpr int TCP_SERVER_BACKLOG = 1;
static constexpr size_t TCP_RX_DUMMY_BUFFER_SIZE = 64;

TcpServer::TcpServer() = default;

TcpServer::~TcpServer() {
    stop();
}

bool TcpServer::start(uint16_t port) {
    if (task_handle != nullptr) {
        ESP_LOGW(TAG, "TCP Server task already running");
        return true;
    }

    listen_port = port;

    BaseType_t result = xTaskCreate(
        &TcpServer::serverTask,
        "tcp_server_task",
        TCP_SERVER_STACK_DEPTH,
        this,
        TCP_SERVER_TASK_PRIORITY,
        &task_handle
    );

    if (result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create TCP server task");
        task_handle = nullptr;
        return false;
    }

    return true;
}

void TcpServer::stop() {
    int c_fd = client_fd.exchange(-1);
    if (c_fd >= 0) {
        close(c_fd);
    }

    int s_fd = server_fd.exchange(-1);
    if (s_fd >= 0) {
        close(s_fd);
    }

    if (task_handle != nullptr) {
        vTaskDelete(task_handle);
        task_handle = nullptr;
    }

    ESP_LOGI(TAG, "TCP server stopped");
}

bool TcpServer::isConnected() const {
    return client_fd.load() >= 0;
}

size_t TcpServer::sendData(const uint8_t* data, size_t len) {
    int fd = client_fd.load();
    if (fd < 0 || data == nullptr || len == 0) {
        return 0;
    }

    ssize_t bytes_sent = send(fd, data, len, MSG_DONTWAIT);
    return (bytes_sent > 0) ? static_cast<size_t>(bytes_sent) : 0;
}

void TcpServer::setOnClientStateChanged(ClientStateCallback callback) {
    client_state_callback = callback;
}

int TcpServer::createListeningSocket(uint16_t port) {
    int listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listen_socket < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return -1;
    }

    int opt = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in listen_addr = {};
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(port);
    listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_socket, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) < 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        close(listen_socket);
        return -1;
    }

    if (listen(listen_socket, TCP_SERVER_BACKLOG) < 0) {
        ESP_LOGE(TAG, "Error during listen: errno %d", errno);
        close(listen_socket);
        return -1;
    }

    return listen_socket;
}

void TcpServer::drainClientSocket(int client_sock) {
    char rx_buffer[TCP_RX_DUMMY_BUFFER_SIZE];
    while (true) {
        int len = recv(client_sock, rx_buffer, sizeof(rx_buffer), 0);
        if (len <= 0) {
            ESP_LOGW(TAG, "Client disconnected or socket closed");
            break;
        }
    }
}

void TcpServer::handleClientSession(int active_sock) {
    int nodelay = 1;
    setsockopt(active_sock, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

    ESP_LOGI(TAG, "Desktop client connected!");
    client_fd.store(active_sock);

    if (client_state_callback) {
        client_state_callback(true);
    }

    drainClientSocket(active_sock);

    close(active_sock);
    client_fd.store(-1);

    if (client_state_callback) {
        client_state_callback(false);
    }
}

void TcpServer::serverTask(void* pvParameters) {
    auto* self = static_cast<TcpServer*>(pvParameters);

    int listen_socket = self->createListeningSocket(self->listen_port);
    if (listen_socket < 0) {
        self->task_handle = nullptr;
        vTaskDelete(NULL);
        return;
    }

    self->server_fd.store(listen_socket);
    ESP_LOGI(TAG, "TCP server listening on port %d", self->listen_port);

    while (true) {
        struct sockaddr_in source_addr = {};
        socklen_t addr_len = sizeof(source_addr);

        int active_sock = accept(listen_socket, (struct sockaddr*)&source_addr, &addr_len);
        if (active_sock < 0) {
            if (self->server_fd.load() < 0) {
                break;
            }
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        self->handleClientSession(active_sock);
    }

    int remaining_s_fd = self->server_fd.exchange(-1);
    if (remaining_s_fd >= 0) {
        close(remaining_s_fd);
    }

    self->task_handle = nullptr;
    vTaskDelete(NULL);
}
