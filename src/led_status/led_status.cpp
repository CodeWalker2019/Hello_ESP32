#include "led_status/led_status.hpp"
#include "ledc_led/ledc_led.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>

namespace {
    constexpr uint32_t kMaxDuty = (1 << LEDC_DUTY_RES) - 1;

    constexpr uint32_t kSearchStepDelayMs     = 15;
    constexpr uint32_t kSearchSteps           = 66;

    constexpr uint32_t kConnectingStepDelayMs = 10;
    constexpr uint32_t kConnectingSteps       = 20;

    constexpr uint32_t kConnectedPollDelayMs  = 200;

    constexpr uint32_t kLedStatusTaskStackSize   = 2048;
    constexpr UBaseType_t kLedStatusTaskPriority = 1;

    volatile led_state_t current_state = LED_STATE_SEARCH;

    void set_duty(uint32_t duty) {
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    }

    bool fade_cycle(uint32_t steps, uint32_t step_delay_ms, led_state_t state_at_entry) {
        for (uint32_t i = 0; i <= steps; i++) {
            if (current_state != state_at_entry) return false;
            set_duty((kMaxDuty * i) / steps);
            vTaskDelay(pdMS_TO_TICKS(step_delay_ms));
        }
        for (uint32_t i = steps; i > 0; i--) {
            if (current_state != state_at_entry) return false;
            set_duty((kMaxDuty * i) / steps);
            vTaskDelay(pdMS_TO_TICKS(step_delay_ms));
        }
        return true;
    }

    void led_status_task(void*) {
        while (true) {
            led_state_t state = current_state;

            switch (state) {
                case LED_STATE_SEARCH:
                    fade_cycle(kSearchSteps, kSearchStepDelayMs, state);
                    break;

                case LED_STATE_CONNECTING:
                    fade_cycle(kConnectingSteps, kConnectingStepDelayMs, state);
                    break;

                case LED_STATE_CONNECTED:
                    set_duty(kMaxDuty);
                    while (current_state == LED_STATE_CONNECTED) {
                        vTaskDelay(pdMS_TO_TICKS(kConnectedPollDelayMs));
                    }
                    break;
            }
        }
    }

}

void led_status_init(void) {
    ledc_led_init();
    xTaskCreate(led_status_task, "led_status", kLedStatusTaskStackSize, nullptr, kLedStatusTaskPriority, nullptr);
}

void led_status_set(led_state_t new_state) {
    current_state = new_state;
}
