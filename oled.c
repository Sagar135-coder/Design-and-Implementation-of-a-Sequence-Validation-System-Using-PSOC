#include "oled.h"

// ========== DIGIT FONT (4x8) ==========
const uint8_t digit_font[16][4] = {
    {0x3E,0x51,0x49,0x3E}, {0x00,0x42,0x7F,0x40},
    {0x62,0x51,0x49,0x46}, {0x22,0x49,0x49,0x36},
    {0x18,0x14,0x12,0x7F}, {0x27,0x45,0x45,0x39},
    {0x3C,0x4A,0x49,0x31}, {0x01,0x71,0x09,0x07},
    {0x36,0x49,0x49,0x36}, {0x26,0x49,0x49,0x3E},
    {0x7E,0x09,0x09,0x7E}, {0x7F,0x49,0x49,0x36},
    {0x3E,0x41,0x41,0x22}, {0x7F,0x41,0x41,0x3E},
    {0x7F,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x01}
};

// ========== INTERNAL LOW-LEVEL FUNCTIONS ==========
static void oled_send_cmd(cyhal_i2c_t *i2c, uint8_t cmd) {
    uint8_t buff[2]={0x00,cmd};
    cyhal_i2c_master_write(i2c, OLED_ADDR, buff, 2, 0, true);
}

static void oled_send_data(cyhal_i2c_t *i2c, uint8_t data) {
    uint8_t buff[2]={0x40,data};
    cyhal_i2c_master_write(i2c, OLED_ADDR, buff, 2, 0, true);
}

// ========== PUBLIC FUNCTIONS ==========
void oled_clear(cyhal_i2c_t *i2c) {
    for(uint8_t page=0;page<8;page++){
        oled_send_cmd(i2c,0xB0+page);
        oled_send_cmd(i2c,0x00);
        oled_send_cmd(i2c,0x10);
        for(int i=0;i<128;i++) oled_send_data(i2c,0x00);
    }
}

void oled_set_cursor(cyhal_i2c_t *i2c, uint8_t col, uint8_t page) {
    oled_send_cmd(i2c,0xB0+page);
    oled_send_cmd(i2c,0x00 + (col & 0x0F));
    oled_send_cmd(i2c,0x10 + (col >> 4));
}

void oled_init(cyhal_i2c_t *i2c) {
    oled_send_cmd(i2c,0xAE);
    oled_send_cmd(i2c,0xD5); oled_send_cmd(i2c,0x80);
    oled_send_cmd(i2c,0xA8); oled_send_cmd(i2c,0x3F);
    oled_send_cmd(i2c,0xD3); oled_send_cmd(i2c,0x00);
    oled_send_cmd(i2c,0x40);
    oled_send_cmd(i2c,0xA1);
    oled_send_cmd(i2c,0xC8);
    oled_send_cmd(i2c,0xDA); oled_send_cmd(i2c,0x12);
    oled_send_cmd(i2c,0x81); oled_send_cmd(i2c,0x7F);
    oled_send_cmd(i2c,0xD9); oled_send_cmd(i2c,0xF1);
    oled_send_cmd(i2c,0xDB); oled_send_cmd(i2c,0x40);
    oled_send_cmd(i2c,0x8D); oled_send_cmd(i2c,0x14);
    oled_send_cmd(i2c,0xA6);
    oled_send_cmd(i2c,0xAF);
}

void oled_print_big_digit(cyhal_i2c_t *i2c, uint8_t col, uint8_t page, uint8_t index){
    if(index>15) return;

    uint8_t top[8],bottom[8];
    for(int i=0;i<4;i++){
        uint8_t byte=digit_font[index][i];
        uint8_t t=0,b=0;

        for(int b_i=0;b_i<8;b_i++){
            if(byte & (1<<b_i)){
                if(b_i<4) t |= (1<<(b_i*2)) | (1<<(b_i*2+1));
                else      b |= (1<<((b_i-4)*2)) | (1<<((b_i-4)*2+1));
            }
        }
        top[i*2]=top[i*2+1]=t;
        bottom[i*2]=bottom[i*2+1]=b;
    }

    oled_set_cursor(i2c,col,page);
    for(int i=0;i<8;i++) oled_send_data(i2c,top[i]);
    oled_set_cursor(i2c,col,page+1);
    for(int i=0;i<8;i++) oled_send_data(i2c,bottom[i]);
}

int char_to_font_index(char key){
    if(key>='0' && key<='9') return key-'0';
    if(key>='A' && key<='F') return 10+(key-'A');
    return -1;
}

void oled_print_decimal_number(cyhal_i2c_t *i2c, uint8_t col, uint8_t page, int val){
    if(val<10)
        oled_print_big_digit(i2c,col,page,val);
    else{
        oled_print_big_digit(i2c,col,page,val/10);
        oled_print_big_digit(i2c,col+8,page,val%10);
    }
}

// ========== LETTER PRINTING ==========
void oled_print_big_letter(cyhal_i2c_t *i2c, uint8_t col, uint8_t page, const uint8_t letter[5]){
    uint8_t top[10], bottom[10];

    for(int i=0;i<5;i++){
        uint8_t byte=letter[i];
        uint8_t t=0,b=0;

        for(int b_i=0;b_i<8;b_i++){
            if(byte&(1<<b_i)){
                if(b_i<4) t |= (1<<(b_i*2)) | (1<<(b_i*2+1));
                else      b |= (1<<((b_i-4)*2)) | (1<<((b_i-4)*2+1));
            }
        }

        top[i*2]=top[i*2+1]=t;
        bottom[i*2]=bottom[i*2+1]=b;
    }

    oled_set_cursor(i2c,col,page);
    for(int i=0;i<10;i++) oled_send_data(i2c,top[i]);
    oled_set_cursor(i2c,col,page+1);
    for(int i=0;i<10;i++) oled_send_data(i2c,bottom[i]);
}

// ========== VALID / INVALID ==========
void valid_generated(cyhal_i2c_t *i2c){
    const uint8_t V[5]={0x07,0x08,0x10,0x08,0x07};
    const uint8_t A[5]={0x1E,0x05,0x05,0x1E,0x00};
    const uint8_t L[5]={0x1F,0x10,0x10,0x10,0x00};
    const uint8_t I[5]={0x11,0x11,0x1F,0x11,0x11};
    const uint8_t D[5]={0x1F,0x11,0x11,0x0E,0x00};

    const uint8_t* letters[]={V,A,L,I,D};

    for(int i=0;i<5;i++)
        oled_print_big_letter(i2c,4+i*12,4,letters[i]);
}

void invalid_generated(cyhal_i2c_t *i2c){
    const uint8_t I[5]={0x11,0x11,0x1F,0x11,0x11};
    const uint8_t N[5]={0x1F,0x02,0x04,0x08,0x1F};
    const uint8_t V[5]={0x07,0x08,0x10,0x08,0x07};
    const uint8_t A[5]={0x1E,0x05,0x05,0x1E,0x00};
    const uint8_t L[5]={0x1F,0x10,0x10,0x10,0x00};
    const uint8_t D[5]={0x1F,0x11,0x11,0x0E,0x00};

    const uint8_t* letters[]={I,N,V,A,L,I,D};

    for(int i=0;i<7;i++)
        oled_print_big_letter(i2c,4+i*12,4,letters[i]);
}