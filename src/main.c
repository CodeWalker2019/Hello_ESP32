#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ledc_led/ledc_led.h"

void app_main(void) {
    ESP_LOGI("LED", "Duty set to: %lu", (unsigned long)LED_BRIGHTNESS_50_PERCENT);
    ledc_led_init();
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LED_BRIGHTNESS_50_PERCENT);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);


    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}