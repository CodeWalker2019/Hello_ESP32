#pragma once

#include <functional>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

class WifiNetifManager {
public:
    using ConnectionStatusCallback = std::function<void(bool isConnected)>;

    WifiNetifManager();
    ~WifiNetifManager();

    // Prevent copying to safeguard netif and event handler ownership
    WifiNetifManager(const WifiNetifManager&) = delete;
    WifiNetifManager& operator=(const WifiNetifManager&) = delete;

    /**
     * @brief Initializes netif handles, loads RAM storage mode, and registers event handlers.
     * @return true if network stack initialized successfully.
     */
    bool init();

    /**
     * @brief Configures APSTA mode and starts the SoftAP "Alive" beacon with a unique SSID.
     * @param softap_ssid The broadcast name for the SoftAP interface.
     * @return true if APSTA mode and SoftAP settings were applied.
     */
    bool startApStaMode(const char* softap_ssid);

    /**
     * @brief Connects station interface using raw Wi-Fi credentials received from WifiTouch.
     * @param sta_config The target Wi-Fi router SSID and password struct.
     * @return true if credentials were queued and connection started.
     */
    bool connectStation(const wifi_config_t& sta_config);

    /**
     * @brief Disconnects the Station interface from the current router.
     */
    void disconnectStation();

    /**
     * @brief Toggles SoftAP beaconing/advertising visibility.
     * @param enabled True to broadcast SSID, false to hide/disable beacon.
     */
    void setBeaconEnabled(bool enabled);

    /**
     * @brief Checks if station interface currently holds a valid IP address.
     */
    bool isStationConnected() const;

    /**
     * @brief Sets a listener for Wi-Fi IP acquisition / disconnection events.
     */
    void setOnConnectionStatusChanged(ConnectionStatusCallback callback);

private:
    esp_netif_t* sta_netif = nullptr;
    esp_netif_t* ap_netif = nullptr;
    wifi_init_config_t cfg;

    bool is_connected = false;
    ConnectionStatusCallback status_callback = nullptr;

    static EventGroupHandle_t s_wifi_event_group;
    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
};
