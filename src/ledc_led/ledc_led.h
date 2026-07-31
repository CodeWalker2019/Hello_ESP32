#ifndef LEDC_LED_H
#define LEDC_LED_H

#include "driver/gptimer.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#define LED_GPIO           GPIO_NUM_2
#define LEDC_MODE          LEDC_LOW_SPEED_MODE
#define LEDC_TIMER         LEDC_TIMER_0
#define LEDC_CHANNEL       LEDC_CHANNEL_0
#define LEDC_DUTY_RES      LEDC_TIMER_13_BIT
#define LEDC_FREQUENCY     5000

#define LED_BRIGHTNESS_50_PERCENT  ((1 << 13) / 2)

#ifdef __cplusplus
extern "C" {
#endif

void ledc_led_init(void);

#ifdef __cplusplus
}
#endif

#endif