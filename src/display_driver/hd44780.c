#include "display_driver/hd44780.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"

static const uint16_t   clear_display_cmd_delay           =  3200;
static const uint16_t   return_home_cmd_delay             =  1600;
static const uint8_t    default_cmd_delay                 =  50;
static const uint32_t   power_on_stabilization_delay      =  50 * 1000; // 50 ms (50k μs)
static const uint8_t    init_step_delay                   =  150;
static const uint16_t   wake_up_sequence_delay            =  5 * 1000;  // 5 ms (5k μs)
static const uint8_t    max_line_chars_length             =  16;
static const uint8_t    screen_lines_available            =  2;
static const uint8_t    total_capacity                    = max_line_chars_length * screen_lines_available;

// -----------------------------------------------------------------------------
// Helper functions
// -----------------------------------------------------------------------------

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

static void perform_pulse_with(int delay) {
    gpio_set_level(LCD_PIN_EN, 1);
    esp_rom_delay_us(1);
    gpio_set_level(LCD_PIN_EN, 0);
    esp_rom_delay_us(delay);
}

static void lcd_set_bus_nibble(uint8_t nibble) {
    gpio_set_level(LCD_PIN_DB4, (nibble >> 0) & 0x01);
    gpio_set_level(LCD_PIN_DB5, (nibble >> 1) & 0x01);
    gpio_set_level(LCD_PIN_DB6, (nibble >> 2) & 0x01);
    gpio_set_level(LCD_PIN_DB7, (nibble >> 3) & 0x01);
}

static void lcd_write_nibble(uint8_t nibble) {
    lcd_set_bus_nibble(nibble & LCD_NIBBLE_MASK);
    perform_pulse_with(default_cmd_delay);
}

static void lcd_write_byte(uint8_t byte) {
    lcd_write_nibble(byte >> 4);
    lcd_write_nibble(byte);     
    esp_rom_delay_us(default_cmd_delay);
}

static void send_cmd_with_delay(uint8_t cmd, int delay) {
    lcd_write_nibble(cmd >> 4);
    perform_pulse_with(default_cmd_delay);
    lcd_write_nibble(cmd);     
    perform_pulse_with(delay);
}

static void set_cursor(uint8_t position) {
    const uint8_t wrapped_pos = position % total_capacity;
    const uint8_t line_base = (wrapped_pos < max_line_chars_length) ? LCD_LINE1_ADDR : LCD_LINE2_ADDR;
    const uint8_t col = wrapped_pos % max_line_chars_length;
    const uint8_t cursor_position_addr = line_base + col;
    lcd_send_cmd(LCD_CMD_CURSOR_POSITION_SET | cursor_position_addr);
}

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------

void lcd_write_str32(const char* str) {
    if (str == NULL) return;

    uint8_t count = 0;
    set_cursor(0);

    while (*str != '\0' && count < total_capacity) {
        if (count == max_line_chars_length) set_cursor(count);
        lcd_send_data((uint8_t)*str++);
        count++;
    }
}

void lcd_send_cmd(uint8_t cmd) {
    gpio_set_level(LCD_PIN_RS, 0);
    
    if (cmd == LCD_CMD_CLEAR_DISPLAY) {
        send_cmd_with_delay(LCD_CMD_CLEAR_DISPLAY, clear_display_cmd_delay);
        return;
    }

    if (cmd == LCD_CMD_FUNCTION_RETURN_HOME) {
        send_cmd_with_delay(LCD_CMD_FUNCTION_RETURN_HOME, return_home_cmd_delay);
        return;
    }
    
    lcd_write_byte(cmd);
}

void lcd_send_data(uint8_t data) {
    gpio_set_level(LCD_PIN_RS, 1);
    lcd_write_byte(data);
}

void lcd_init(void) {
    setup_gpio();
    esp_rom_delay_us(power_on_stabilization_delay);

    // Ensure baseline control state
    gpio_set_level(LCD_PIN_RS, 0);
    gpio_set_level(LCD_PIN_EN, 0);

    // Wakeup Sequence (Single Nibble Commands)
    lcd_set_bus_nibble(LCD_CMD_WAKEUP);
    perform_pulse_with(default_cmd_delay);
    esp_rom_delay_us(wake_up_sequence_delay);
    perform_pulse_with(init_step_delay);
    perform_pulse_with(init_step_delay);

    // Switch Hardware Bus to 4-Bit Mode ---
    lcd_set_bus_nibble(LCD_CMD_SET_4BIT);
    perform_pulse_with(init_step_delay);

    // Datasheet Instructions
    lcd_send_cmd(LCD_CMD_FUNCTION_SET);
    lcd_send_cmd(LCD_CMD_DISPLAY_OFF); 
    lcd_send_cmd(LCD_CMD_CLEAR_DISPLAY);
    lcd_send_cmd(LCD_CMD_ENTRY_MODE_SET);
    lcd_send_cmd(LCD_CMD_DISPLAY_ON);
}
