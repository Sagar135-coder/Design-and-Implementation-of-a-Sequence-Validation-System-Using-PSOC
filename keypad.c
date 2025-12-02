#include "keypad.h"

static uint32_t ROW[4];
static uint32_t COL[4];

static const char keymap[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'0','E','F','D'}
};

// Driver state

void keypad_init(const uint32_t row_pins[4], const uint32_t col_pins[4])
{
    for (int i = 0; i < 4; i++) {
        ROW[i] = row_pins[i];
        COL[i] = col_pins[i];
    }

    // Rows as output HIGH
    for (int i = 0; i < 4; i++) {
        cyhal_gpio_init(ROW[i], CYHAL_GPIO_DIR_OUTPUT,
                        CYHAL_GPIO_DRIVE_STRONG, true);
    }

    // Columns as input with pullups
    for (int i = 0; i < 4; i++) {
        cyhal_gpio_init(COL[i], CYHAL_GPIO_DIR_INPUT,
                        CYHAL_GPIO_DRIVE_PULLUP, true);
    }
}

char keypad_get_key(void)
{

    for (int r = 0; r < 4; r++)
    {
        cyhal_gpio_write(ROW[r], false); // Activate row

        for (int c = 0; c < 4; c++)
        {
            if (cyhal_gpio_read(COL[c]) == false)
            {
                cyhal_system_delay_ms(20);
                while (cyhal_gpio_read(COL[c]) == false);

                return keymap[r][c];
            }
        }

        cyhal_gpio_write(ROW[r], true); // Deactivate row
    }
    return 0;
}