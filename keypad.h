#ifndef KEYPAD_H
#define KEYPAD_H

#include "cyhal.h"
#include "cybsp.h"

void keypad_init(const uint32_t row_pins[4], const uint32_t col_pins[4]);
char keypad_get_key(void);

#endif
