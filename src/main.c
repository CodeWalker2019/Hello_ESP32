#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ledc_led/ledc_led.h"
#include "esp_log.h"

void app_main(void) {
    ledc_led_init();

    while (1) {
        for (int duty = 0; duty <= 8191; duty += 250) {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(25));
        }

        for (int duty = 8191; duty >= 0; duty -= 250) {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(25));
        }
    }
}