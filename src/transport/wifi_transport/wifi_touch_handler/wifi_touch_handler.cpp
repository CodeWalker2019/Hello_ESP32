#include "transport/wifi_transport/wifi_touch_handler.hpp"
#include "esp_smartconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include <cstring>

static const char* WIFI_TOUCH_LOGS = "WifiTouch";

WifiTouch::WifiTouch() {}

WifiTouch::~WifiTouch() {
    stop();
}

esp_err_t WifiTouch::start() {
    if (isSniffing()) {
        ESP_LOGE(WIFI_TOUCH_LOGS, "Error starting WifiTouch: sniffer already running!");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_RETURN_ON_ERROR(
        esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &WifiTouch::event_handler, this),
        WIFI_TOUCH_LOGS,
        "Failed to register SC Event"
    );

    esp_err_t ret = ESP_OK;

    ESP_GOTO_ON_ERROR(
        esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_V2),
        err_cleanup,
        WIFI_TOUCH_LOGS,
        "Failed to set SC_TYPE_ESPTOUCH_V2"
    );

    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();

    ESP_GOTO_ON_ERROR(
        esp_smartconfig_start(&cfg),
        err_cleanup,
        WIFI_TOUCH_LOGS,
        "Failed to start smart config"
    );

    isSnifferActive.store(true);
    return ESP_OK;

err_cleanup:
    esp_event_handler_unregister(SC_EVENT, ESP_EVENT_ANY_ID, &WifiTouch::event_handler);
    return ret;
}

esp_err_t WifiTouch::stop() {
    if (!isSniffing()) {
        return ESP_OK;
    }

    esp_err_t err = esp_smartconfig_stop();

    if (err != ESP_OK) {
        ESP_LOGE(WIFI_TOUCH_LOGS, "Failed to stop smart config");
        esp_event_handler_unregister(SC_EVENT, ESP_EVENT_ANY_ID, &WifiTouch::event_handler);
        return err;
    }

    isSnifferActive.store(false);
    err = esp_event_handler_unregister(SC_EVENT, ESP_EVENT_ANY_ID, &WifiTouch::event_handler);

    if (err != ESP_OK) {
        ESP_LOGE(WIFI_TOUCH_LOGS, "Failed to unregister handler");
    }

    return err;
}

esp_err_t WifiTouch::sendAck() {
    ESP_RETURN_ON_ERROR(
        esp_smartconfig_fast_resp(),
        WIFI_TOUCH_LOGS,
        "Failed to send ack"
    );
    return ESP_OK;
}

bool WifiTouch::isSniffing() const {
    return isSnifferActive;
}

void WifiTouch::setOnCredentialsReceived(CredentialsCallback credentials_handler) {
    this->credentials_handler = std::move(credentials_handler);
}

void WifiTouch::event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base != SC_EVENT) {
        return;
    }

    WifiTouch* touch = static_cast<WifiTouch*>(arg);
    if (!touch) {
        return;
    }

    switch (event_id) {
        case SC_EVENT_SCAN_DONE:
            ESP_LOGI(WIFI_TOUCH_LOGS, "Scan completed");
            break;

        case SC_EVENT_FOUND_CHANNEL:
            ESP_LOGI(WIFI_TOUCH_LOGS, "Locked onto channel");
            break;

        case SC_EVENT_GOT_SSID_PSWD: {
            ESP_LOGI(WIFI_TOUCH_LOGS, "Got SSID and password");

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
            ESP_LOGI(WIFI_TOUCH_LOGS, "ACK send completed");
            touch->stop();
            break;

        default:
            break;
    }
}
