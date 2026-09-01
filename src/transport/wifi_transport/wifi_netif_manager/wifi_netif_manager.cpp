#include "transport/wifi_transport/wifi_netif_manager/wifi_netif_manager.hpp"
#include <esp_log.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

namespace {
    constexpr uint16_t kBeaconPort = 7001;
    constexpr uint32_t kBeaconIntervalMs = 1000;
    constexpr uint32_t kBeaconTaskStackSize = 3072;
    constexpr UBaseType_t kBeaconTaskPriority = 1;
}

static const char* TAG = "WifiNetifManager";

EventGroupHandle_t WifiNetifManager::s_wifi_event_group = nullptr;

WifiNetifManager::WifiNetifManager()
    : sta_netif(nullptr),
      ap_netif(nullptr),
      cfg(WIFI_INIT_CONFIG_DEFAULT()) {
    if (s_wifi_event_group == nullptr) {
        s_wifi_event_group = xEventGroupCreate();
    }
}

WifiNetifManager::~WifiNetifManager() {
    if (beacon_task_handle) {
        vTaskDelete(beacon_task_handle);
        beacon_task_handle = nullptr;
    }
    if (sta_netif) {
        esp_netif_destroy_default_wifi(sta_netif);
        sta_netif = nullptr;
    }
    if (ap_netif) {
        esp_netif_destroy_default_wifi(ap_netif);
        ap_netif = nullptr;
    }
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler);
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler);
}

bool WifiNetifManager::init() {
    sta_netif = esp_netif_create_default_wifi_sta();
    ap_netif = esp_netif_create_default_wifi_ap();

    if (sta_netif == nullptr || ap_netif == nullptr) {
        ESP_LOGE(TAG, "Failed to create netif instances");
        return false;
    }

    if (esp_wifi_init(&cfg) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize esp_wifi");
        return false;
    }

    if (esp_wifi_set_storage(WIFI_STORAGE_RAM) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WIFI_STORAGE_RAM");
        return false;
    }

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, this);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, this);

    return true;
}

bool WifiNetifManager::startApStaMode(const char* softap_ssid) {
    if (softap_ssid == nullptr) {
        ESP_LOGE(TAG, "SSID string is null");
        return false;
    }

    if (esp_wifi_set_mode(WIFI_MODE_APSTA) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set APSTA mode");
        return false;
    }

    wifi_config_t ap_config = {};
    strncpy((char*)ap_config.ap.ssid, softap_ssid, sizeof(ap_config.ap.ssid) - 1);
    ap_config.ap.ssid_len = strlen(softap_ssid);
    ap_config.ap.max_connection = 4;
    ap_config.ap.authmode = WIFI_AUTH_OPEN;

    if (esp_wifi_set_config(WIFI_IF_AP, &ap_config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set SoftAP configuration");
        return false;
    }

    if (esp_wifi_start() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Wi-Fi driver");
        return false;
    }

    strncpy(beacon_message, softap_ssid, sizeof(beacon_message) - 1);
    xTaskCreate(&WifiNetifManager::beaconTask, "wifi_udp_beacon", kBeaconTaskStackSize, this,
                kBeaconTaskPriority, &beacon_task_handle);

    ESP_LOGI(TAG, "APSTA mode started with SoftAP SSID: %s", softap_ssid);
    return true;
}

bool WifiNetifManager::connectStation(const wifi_config_t& sta_config) {
    esp_wifi_disconnect();

    if (esp_wifi_set_config(WIFI_IF_STA, const_cast<wifi_config_t*>(&sta_config)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to apply station configuration");
        return false;
    }

    if (esp_wifi_connect() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initiate station connection");
        return false;
    }

    ESP_LOGI(TAG, "Station connecting to router SSID: %s", sta_config.sta.ssid);
    return true;
}

void WifiNetifManager::disconnectStation() {
    esp_wifi_disconnect();
    is_connected = false;
    ESP_LOGI(TAG, "Station disconnected from AP");
}

void WifiNetifManager::setBeaconEnabled(bool enabled) {
    beacon_enabled = enabled;

    wifi_config_t ap_config = {};
    if (esp_wifi_get_config(WIFI_IF_AP, &ap_config) == ESP_OK) {
        ap_config.ap.ssid_hidden = enabled ? 0 : 1;
        esp_wifi_set_config(WIFI_IF_AP, &ap_config);
        ESP_LOGI(TAG, "SoftAP beacon visibility set to: %s", enabled ? "visible" : "hidden");
    }
}

bool WifiNetifManager::isStationConnected() const {
    return is_connected;
}

void WifiNetifManager::setOnConnectionStatusChanged(ConnectionStatusCallback callback) {
    status_callback = callback;
}

void WifiNetifManager::event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    auto* self = static_cast<WifiNetifManager*>(arg);

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        auto* event = static_cast<ip_event_got_ip_t*>(event_data);
        ESP_LOGI(TAG, "Acquired IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
        
        self->is_connected = true;
        if (self->status_callback) {
            self->status_callback(true);
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "Station disconnected from AP, retrying...");

        self->is_connected = false;
        if (self->status_callback) {
            self->status_callback(false);
        }

        esp_wifi_connect();
    }
}

void WifiNetifManager::beaconTask(void* arg) {
    auto* self = static_cast<WifiNetifManager*>(arg);

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create UDP beacon socket");
        vTaskDelete(nullptr);
        return;
    }

    int broadcast_enable = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable));

    sockaddr_in dest_addr = {};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(kBeaconPort);
    dest_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    while (true) {
         if (self->beacon_enabled && self->is_connected) {
            size_t len = strnlen(self->beacon_message, sizeof(self->beacon_message));
            sendto(sock, self->beacon_message, len, 0, reinterpret_cast<sockaddr*>(&dest_addr),
                   sizeof(dest_addr));
        }
        vTaskDelay(pdMS_TO_TICKS(kBeaconIntervalMs));
    }
}
