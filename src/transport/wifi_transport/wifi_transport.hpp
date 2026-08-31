#pragma once

#include "transport/i_transport.hpp"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_TRANSPORT_LOGS     "WifiTransport";

class WifiTransport : public ITransport {
public:
    WifiTransport();
    ~WifiTransport() override;

    bool init() override;
    bool isReady() const override;
    void setBeaconEnabled(bool enabled) override;
    size_t send(const uint8_t* data, size_t len) override;
    void setOnStateChangeListener(StateChangeCallback callback) override;

private:
    esp_netif_t* sta_netif = nullptr;
    wifi_init_config_t cfg;
    StateChangeCallback stateChangeCallback = nullptr;
    
    static EventGroupHandle_t s_wifi_event_group;
    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
};
