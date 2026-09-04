#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sys_init/init_wifi_nvs_deterministic.h"
#include "led_status/led_status.hpp"
#include "sensors/motion_sensor.h"
#include "display_driver/hd44780.h"
#include "system_orchestrator.hpp"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"

void app_main(void) {
    lcd_init();

    ESP_ERROR_CHECK(init_wifi_nvs_deterministic());

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    led_status_init();
    motion_sensor_init();
    motion_sensor_check_present();

    ESP_ERROR_CHECK(system_orchestrator_init());

    vTaskDelete(NULL);
}

