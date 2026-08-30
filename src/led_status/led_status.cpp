#include "led_status/led_status.hpp"
#include "ledc_led/ledc_led.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <atomic>
#include <esp_log.h>

namespace {
    constexpr uint32_t kMaxDuty = (1 << LEDC_DUTY_RES) - 1;

    constexpr uint32_t kSearchStepDelayMs     = 15;
    constexpr uint32_t kSearchSteps           = 66;

    constexpr uint32_t kConnectingStepDelayMs = 10;
    constexpr uint32_t kConnectingSteps       = 20;

    constexpr uint32_t kLedStatusTaskStackSize   = 1536;
    constexpr UBaseType_t kLedStatusTaskPriority = 1;

    std::atomic<led_state_t> s_current_state{LED_STATE_SEARCH};
    TaskHandle_t s_led_task_handle = nullptr;
    StaticTask_t s_led_task_tcb;
    StackType_t  s_led_task_stack[kLedStatusTaskStackSize / sizeof(StackType_t)];

    inline void set_duty(uint32_t duty) {
        ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
        ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    }

    bool fade_cycle(uint32_t steps, uint32_t step_delay_ms, led_state_t state_at_entry) {
        for (uint32_t i = 0; i <= steps; ++i) {
            if (s_current_state.load(std::memory_order_relaxed) != state_at_entry) {
                return false;
            }
            set_duty((kMaxDuty * i) / steps);
            vTaskDelay(pdMS_TO_TICKS(step_delay_ms));
        }
        for (uint32_t i = steps; i > 0; --i) {
            if (s_current_state.load(std::memory_order_relaxed) != state_at_entry) {
                return false;
            }
            set_duty((kMaxDuty * i) / steps);
            vTaskDelay(pdMS_TO_TICKS(step_delay_ms));
        }
        return true;
    }

    void led_status_task(void*) {
        while (true) {
            led_state_t state = s_current_state.load(std::memory_order_relaxed);

            switch (state) {
                case LED_STATE_SEARCH:
                    fade_cycle(kSearchSteps, kSearchStepDelayMs, state);
                    break;

                case LED_STATE_CONNECTING:
                    fade_cycle(kConnectingSteps, kConnectingStepDelayMs, state);
                    break;

                case LED_STATE_CONNECTED:
                    set_duty(kMaxDuty);
                    while (s_current_state.load(std::memory_order_relaxed) == LED_STATE_CONNECTED) {
                        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
                    }
                    break;
            }
        }
    }
} // namespace

void led_status_init(void) {
    ledc_led_init();
    s_led_task_handle = xTaskCreateStatic(
        led_status_task,
        "led_status",
        sizeof(s_led_task_stack) / sizeof(StackType_t),
        nullptr,
        kLedStatusTaskPriority,
        s_led_task_stack,
        &s_led_task_tcb
    );
}

void led_status_set(led_state_t new_state) {
    s_current_state.store(new_state, std::memory_order_relaxed);
    if (s_led_task_handle != nullptr) {
        xTaskNotifyGive(s_led_task_handle);
    }
}

