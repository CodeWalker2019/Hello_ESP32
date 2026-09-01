#include "transport/wifi_transport/wifi_transport.hpp"
#include <esp_log.h>
#include <cstdio>
#include "app_config.h"

static const char* TAG = "WifiTransport";

WifiTransport::WifiTransport() = default;

WifiTransport::~WifiTransport() = default;

bool WifiTransport::init() {
    if (!netifManager.init()) {
        ESP_LOGE(TAG, "Failed to initialize NetifManager");
        return false;
    }

    if (!netifManager.startApStaMode(CONFIG_DEFAULT_SOFTAP_SSID)) {
        ESP_LOGE(TAG, "Failed to start APSTA mode");
        return false;
    }

    netifManager.setOnConnectionStatusChanged([this](bool isConnected) {
        handleConnectionStatusChanged(isConnected);
    });

    touchHandler.setOnCredentialsReceived([this](const wifi_config_t& wifi_cfg) {
        handleCredentialsReceived(wifi_cfg);
    });

    tcpServer.setOnClientStateChanged([this](bool isConnected) {
        handleClientStateChanged(isConnected);
    });

    if (touchHandler.start() != ESP_OK) {
        ESP_LOGW(TAG, "Failed to start WifiTouch sniffer (might already be running)");
    }

    if (!tcpServer.start(8080)) {
        ESP_LOGE(TAG, "Failed to start TCP server");
        return false;
    }

    ESP_LOGI(TAG, "WifiTransport successfully initialized");
    return true;
}

bool WifiTransport::isReady() const {
    return tcpServer.isConnected();
}

void WifiTransport::setBeaconEnabled(bool enabled) {
    netifManager.setBeaconEnabled(enabled);
    
    if (!enabled && isReady()) {
        touchHandler.stop();
    }
}

size_t WifiTransport::send(const uint8_t* data, size_t len) {
    if (!isReady()) {
        return 0;
    }
    return tcpServer.sendData(data, len);
}

void WifiTransport::setOnStateChangeListener(StateChangeCallback callback) {
    stateChangeCallback = callback;
    
    if (stateChangeCallback) {
        stateChangeCallback(this, isReady());
    }
}

void WifiTransport::handleCredentialsReceived(const wifi_config_t& wifi_cfg) {
    ESP_LOGI(TAG, "Credentials received for SSID: %s, connecting...", wifi_cfg.sta.ssid);
    netifManager.connectStation(wifi_cfg);
}

void WifiTransport::handleConnectionStatusChanged(bool isConnected) {
    if (isConnected) {
        ESP_LOGI(TAG, "Station acquired IP address. Sending ESPTouch ACK confirmation...");
        touchHandler.sendAck();
    } else {
        ESP_LOGW(TAG, "Station interface disconnected");
    }
}

void WifiTransport::handleClientStateChanged(bool isConnected) {
    if (isConnected) {
        ESP_LOGI(TAG, "Desktop TCP client connected. Telemetry streaming ready.");
        touchHandler.stop();
    } else {
        ESP_LOGW(TAG, "Desktop TCP client disconnected. Resuming SmartConfig discovery...");
        touchHandler.start();
    }

    if (stateChangeCallback) {
        stateChangeCallback(this, isConnected);
    }
}
