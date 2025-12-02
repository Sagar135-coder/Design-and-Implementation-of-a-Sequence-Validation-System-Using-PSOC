#include "cyhal.h"
#include "cybsp.h"
#include "keypad.h"
#include "oled.h"

#define SEQ_LEN 6

// ---------------- Sequence Logic ----------------
int validate_sequence(int *seq){
    for(int i=3;i<6;i++){
        if(seq[i] != seq[i-2] + seq[i-3])
            return 0;
    }
    return 1;
}

void generate_next(int *seq,int *next){
    next[0] = seq[4] + seq[3];
    next[1] = seq[5] + seq[4];
    next[2] = next[0] + seq[5];
}

// ---------------- MAIN ----------------
int main(void){
    cybsp_init();
    __enable_irq();

    cyhal_i2c_t i2c;
    cyhal_i2c_cfg_t cfg={.is_slave=false,.address=0,.frequencyhal_hz=400000};

    cyhal_i2c_init(&i2c, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    cyhal_i2c_configure(&i2c,&cfg);

    oled_init(&i2c);
    oled_clear(&i2c);

    const uint8_t MINUS[5]={0x00,0x04,0x04,0x04,0x00};

    uint32_t row_pins[4]={P10_0,P10_3,P10_4,P10_1};
    uint32_t col_pins[4]={P9_0,P9_1,P9_2,P9_6};
    keypad_init(row_pins,col_pins);

    char key;
    int seq[6], next[3];
    int count=0;

    // -------- INPUT FROM KEYPAD --------
    while(count<6){
        key = keypad_get_key();
        if(key){
            int val = char_to_font_index(key);
            if(val!=-1){
                seq[count] = val;

                // First 3 numbers on page 2
                if(count < 3)
                    oled_print_decimal_number(&i2c, 4 + count*20, 2, val);
                else
                    oled_print_decimal_number(&i2c, 4 + (count-3)*20, 4, val);

                count++;
                cyhal_system_delay_ms(180);
            }
        }
    }

    // -------- PROCESS --------
    oled_clear(&i2c);

    if(!validate_sequence(seq)){
        invalid_generated(&i2c);
        oled_print_big_letter(&i2c,4,2,MINUS);
        oled_print_big_digit(&i2c,20,2,1);
        while(1);
    }

    generate_next(seq,next);
    valid_generated(&i2c);

    for(int i=0;i<3;i++)
        oled_print_decimal_number(&i2c,4+i*20,2,next[i]);

    while(1);
}