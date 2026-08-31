#pragma once

#include <functional>
#include "esp_err.h"
#include "esp_wifi.h"
#include <atomic>
#include "esp_event.h"

class WifiTouch {
    public:
        using CredentialsCallback = std::function<void(const wifi_config_t&)>;

        WifiTouch() = default;
        ~WifiTouch();
    
        /**
        * @brief Configures SmartConfig for ESPTouch V2, registers event handlers with the default event loop, and starts channel hopping/sniffing.
        * @note Wi-Fi driver must already be initialized and started (esp_wifi_start()).
        * @return ESP_OK on successful start, ESP_ERR_INVALID_STATE if already running, or other esp_err_t on initialization failure.
        */
        esp_err_t start();
        
        /**
        * @brief Stops the sniffer, frees SmartConfig buffers, and unregisters event listeners.
        * @return ESP_OK if stopped cleanly.
        */
        esp_err_t stop();
        
        /**
        * @brief Sends the acknowledgment packet back to the provisioning app (fast response).
        * @note Must only be invoked after the station has successfully acquired an IP address (IP_EVENT_STA_GOT_IP).
        * @return ESP_OK if the ACK transmission was initiated.
        */
        esp_err_t sendAck();
        
        /**
        * @brief Thread-safe check of whether the sniffer is actively listening.
        * @return true if active, false otherwise.
        */
        bool isSniffing() const;
        
        /**
        * @brief Registers the listener invoked when valid Wi-Fi credentials have been decoded from the air.
        * @note this callback is executed from the ESP-IDF default event loop task context, so callers must avoid blocking operations inside it.
        * @param credentials_handler The callback function receiving the populated wifi_config_t.
        */
        void setOnCredentialsReceived(CredentialsCallback credentials_handler);

    private:
        std::atomic<bool> isSnifferActive { false };
        CredentialsCallback credentials_handler;
        static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
};
