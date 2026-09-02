#include "display_driver/hd44780.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_rom_sys.h"

static const char *TAG = "LCD_MAIN";

static void setup_gpio(void) {
    uint64_t pin_mask = (1ULL << LCD_PIN_RS)  | (1ULL << LCD_PIN_EN)  |
                        (1ULL << LCD_PIN_DB4) | (1ULL << LCD_PIN_DB5) |
                        (1ULL << LCD_PIN_DB6) | (1ULL << LCD_PIN_DB7);

    gpio_config_t io_conf = {
        .pin_bit_mask = pin_mask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

static void perform_pulse(void) {
    gpio_set_level(LCD_PIN_EN, 1);
    esp_rom_delay_us(1);
    gpio_set_level(LCD_PIN_EN, 0);
    esp_rom_delay_us(50);
}

static void set_data_nibble(uint8_t nibble) {
    gpio_set_level(LCD_PIN_DB4, (nibble >> 0) & 0x01);
    gpio_set_level(LCD_PIN_DB5, (nibble >> 1) & 0x01);
    gpio_set_level(LCD_PIN_DB6, (nibble >> 2) & 0x01);
    gpio_set_level(LCD_PIN_DB7, (nibble >> 3) & 0x01);
}

static void lcd_send_cmd(uint8_t cmd) {
    gpio_set_level(LCD_PIN_RS, 0); // RS = 0 for Instruction/Command

    set_data_nibble((cmd >> 4) & LCD_NIBBLE_MASK);
    perform_pulse();

    set_data_nibble(cmd & LCD_NIBBLE_MASK);
    perform_pulse();

    esp_rom_delay_us(50);
}

void init_lcd(void) {
    setup_gpio();

    // 1. Power-on stabilization delay (> 40ms)
    vTaskDelay(pdMS_TO_TICKS(50));

    // Ensure baseline control state
    gpio_set_level(LCD_PIN_RS, 0);
    gpio_set_level(LCD_PIN_EN, 0);

    // Wakeup Sequence (Single Nibble Commands)
    set_data_nibble(LCD_CMD_WAKEUP);
    perform_pulse();
    vTaskDelay(pdMS_TO_TICKS(5));
    perform_pulse();
    esp_rom_delay_us(150);
    perform_pulse();
    esp_rom_delay_us(150);

    // Switch Hardware Bus to 4-Bit Mode ---
    set_data_nibble(LCD_CMD_SET_4BIT);
    perform_pulse();
    esp_rom_delay_us(100);

    // Datasheet Instructions
    lcd_send_cmd(LCD_CMD_FUNCTION_SET);
    lcd_send_cmd(LCD_CMD_DISPLAY_OFF); 
    lcd_send_cmd(LCD_CMD_CLEAR_DISPLAY);
    vTaskDelay(pdMS_TO_TICKS(2));
    lcd_send_cmd(LCD_CMD_ENTRY_MODE_SET);
    lcd_send_cmd(LCD_CMD_DISPLAY_ON);
}
