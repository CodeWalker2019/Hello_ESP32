#pragma once

/* 
* Datasheet document used for implementation
* https://cdn.sparkfun.com/assets/9/5/f/7/b/HD44780.pdf
*/

#define LCD_PIN_RS               GPIO_NUM_19
#define LCD_PIN_EN               GPIO_NUM_23
#define LCD_PIN_DB4              GPIO_NUM_18
#define LCD_PIN_DB5              GPIO_NUM_17
#define LCD_PIN_DB6              GPIO_NUM_16
#define LCD_PIN_DB7              GPIO_NUM_15
#define LCD_NIBBLE_MASK          0x0F

// -----------------------------------------------------------------------------
// HD44780 4-Bit Data Bus Line Masks
// -----------------------------------------------------------------------------
#define LCD_NIBBLE_DB4           (1 << 0)  // 0b0001
#define LCD_NIBBLE_DB5           (1 << 1)  // 0b0010
#define LCD_NIBBLE_DB6           (1 << 2)  // 0b0100
#define LCD_NIBBLE_DB7           (1 << 3)  // 0b1000

// -----------------------------------------------------------------------------
// Flowchart Nibble Commands (Used during Wakeup / Mode Switch)
// -----------------------------------------------------------------------------
#define LCD_CMD_WAKEUP           (LCD_NIBBLE_DB5 | LCD_NIBBLE_DB4) // 0b0011
#define LCD_CMD_SET_4BIT         (LCD_NIBBLE_DB5)                 // 0b0010

// -----------------------------------------------------------------------------
// Full 8-Bit Instruction Set Commands (Datasheet Table)
// -----------------------------------------------------------------------------
#define LCD_CMD_FUNCTION_SET          0x28  // 4-Bit Bus, 2 Lines, 5x8 Font
#define LCD_CMD_DISPLAY_OFF           0x08  // Display OFF, Cursor OFF, Blink OFF
#define LCD_CMD_CLEAR_DISPLAY         0x01  // Clear Memory and Reset Cursor
#define LCD_CMD_ENTRY_MODE_SET        0x06  // Auto-Increment Cursor, Shift OFF
#define LCD_CMD_DISPLAY_ON            0x0C  // Display ON, Cursor OFF, Blink OFF        
#define LCD_CMD_FUNCTION_RETURN_HOME  0x02  // Returns display from being shifted
                                            //  to original position.

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------
void lcd_init(void);
void lcd_send_cmd(uint8_t cmd);
void lcd_send_data(uint8_t data);
