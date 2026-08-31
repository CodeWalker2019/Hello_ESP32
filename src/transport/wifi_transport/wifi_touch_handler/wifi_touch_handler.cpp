#include "transport/wifi_transport/wifi_touch_handler/wifi_touch_handler.hpp"
#include "esp_smartconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include <cstring>

static const char* TAG = "WifiTouch";

WifiTouch::~WifiTouch() {
    stop();
}

esp_err_t WifiTouch::start() {
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    esp_err_t ret = ESP_OK;

    ret = esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_V2);
    ESP_GOTO_ON_ERROR(ret, err_cleanup, TAG, "Failed to set smartconfig type");

    ret = esp_smartconfig_start(&cfg);
    ESP_GOTO_ON_ERROR(ret, err_cleanup, TAG, "Failed to start smartconfig");

    isSnifferActive.store(true);
    ESP_LOGI(TAG, "WifiTouch sniffer started successfully");
    return ESP_OK;

err_cleanup:
    esp_smartconfig_stop();
    return ret;
}

esp_err_t WifiTouch::stop() {
    if (!isSniffing()) {
        return ESP_OK;
    }

    esp_err_t err = esp_smartconfig_stop();

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop smart config");
        esp_event_handler_unregister(SC_EVENT, ESP_EVENT_ANY_ID, &WifiTouch::event_handler);
        return err;
    }

    isSnifferActive.store(false);
    err = esp_event_handler_unregister(SC_EVENT, ESP_EVENT_ANY_ID, &WifiTouch::event_handler);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to unregister handler");
    }

    return err;
}

esp_err_t WifiTouch::sendAck() {
    ESP_LOGI(TAG, "Provisioning handshake complete");
    return ESP_OK;
}

bool WifiTouch::isSniffing() const {
    return isSnifferActive.load();
}

void WifiTouch::setOnCredentialsReceived(CredentialsCallback credentials_handler) {
    this->credentials_handler = std::move(credentials_handler);
}

void WifiTouch::event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base != SC_EVENT) {
        return;
    }

    auto* touch = static_cast<WifiTouch*>(arg);
    if (!touch) {
        return;
    }

    switch (event_id) {
        case SC_EVENT_SCAN_DONE:
            ESP_LOGI(TAG, "Scan completed");
            break;

        case SC_EVENT_FOUND_CHANNEL:
            ESP_LOGI(TAG, "Locked onto channel");
            break;

        case SC_EVENT_GOT_SSID_PSWD: {
            ESP_LOGI(TAG, "Got SSID and password");

            auto* evt = static_cast<smartconfig_event_got_ssid_pswd_t*>(event_data);
            wifi_config_t wifi_config;
            memset(&wifi_config, 0, sizeof(wifi_config_t));

            memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
            memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));

            if (touch->credentials_handler) {
                touch->credentials_handler(wifi_config);
            }
            break;
        }

        case SC_EVENT_SEND_ACK_DONE:
            ESP_LOGI(TAG, "ACK send completed");
            touch->stop();
            break;

        default:
            break;
    }
}
