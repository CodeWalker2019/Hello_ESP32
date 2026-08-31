#include "transport/wifi_transport/wifi_netif_manager/wifi_netif_manager.hpp"
#include <esp_log.h>
#include <cstring>

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

    ESP_LOGI(TAG, "APSTA mode started with SoftAP SSID: %s", softap_ssid);
    return true;
}

bool WifiNetifManager::connectStation(const wifi_config_t& sta_config) {
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
        ESP_LOGW(TAG, "Station disconnected from AP");
        
        self->is_connected = false;
        if (self->status_callback) {
            self->status_callback(false);
        }
    }
}
