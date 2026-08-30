#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sys_init/init_wifi_nvs_deterministic.h"
#include "app_bridge.hpp"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"

void app_main(void) {
    ESP_ERROR_CHECK(init_wifi_nvs_deterministic());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(system_orchestrator_init());
}
