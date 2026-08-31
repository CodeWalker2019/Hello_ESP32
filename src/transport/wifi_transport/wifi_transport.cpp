#include "transport/wifi_transport/wifi_transport.hpp"
#include <esp_log.h>

EventGroupHandle_t WifiTransport::s_wifi_event_group = nullptr;

WifiTransport::WifiTransport()
    : sta_netif(nullptr),
      cfg(WIFI_INIT_CONFIG_DEFAULT()) {
    if (s_wifi_event_group == nullptr) {
        s_wifi_event_group = xEventGroupCreate();
    }
}

WifiTransport::~WifiTransport() {}

bool WifiTransport::init() {
    sta_netif = esp_netif_create_default_wifi_sta();
    if (sta_netif == nullptr) {
        ESP_LOGE(WIFI_TRANSPORT_LOGS, "Failed to create default STA netif");
        return false;
    }

    if (esp_wifi_init(&cfg) != ESP_OK) return false;

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, this);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, this);

    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK) return false;
    if (esp_wifi_start() != ESP_OK) return false;

    return true;
}

bool WifiTransport::isReady() const {
    // TODO: Check event group bits or socket connectivity
    // TODO: Implement based on IP / TCP connection state
    return false;
}

void WifiTransport::setBeaconEnabled(bool enabled) {
    // TODO: Enable/disable smartconfig or advertising if inactive
}

size_t WifiTransport::send(const uint8_t* data, size_t len) {
    if (!isReady()) return 0;
    // TODO: Transmit over socket...
    return len;
}

void WifiTransport::setOnStateChangeListener(StateChangeCallback callback) {
    stateChangeCallback = callback;
}

void WifiTransport::event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    auto* self = static_cast<WifiTransport*>(arg);
    
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(WIFI_TRANSPORT_LOGS, "Wi-Fi got IP!");
        self->stateChangeCallback(self, true);
        return;
    } 
    
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(WIFI_TRANSPORT_LOGS, "Wi-Fi disconnected");
        self->stateChangeCallback(self, false);
    }
}