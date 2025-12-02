#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H

#include "cyhal.h"
#include <stdint.h>

#define OLED_ADDR 0x3C

// ---- Public OLED API ----
void oled_init(cyhal_i2c_t *i2c);
void oled_clear(cyhal_i2c_t *i2c);
void oled_set_cursor(cyhal_i2c_t *i2c, uint8_t col, uint8_t page);
void oled_print_big_digit(cyhal_i2c_t *i2c, uint8_t col, uint8_t page, uint8_t index);
void oled_print_decimal_number(cyhal_i2c_t *i2c, uint8_t col, uint8_t page, int val);
int char_to_font_index(char key);

void oled_print_big_letter(cyhal_i2c_t *i2c, uint8_t col, uint8_t page, const uint8_t letter[5]);
void valid_generated(cyhal_i2c_t *i2c);
void invalid_generated(cyhal_i2c_t *i2c);

#endif