#include "../../src/include/font.h"
#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"

struct __attribute__((packed)) textmode {
    uint16_t width;
    uint16_t height;
    uint16_t x_position;
    uint16_t y_position;
};

typedef struct textmode textmode;

struct __attribute__((packed)) framebuffer {
    uint32_t* address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
};

typedef struct framebuffer framebuffer;



void put_pixel(int x, int y, uint32_t color) {
    framebuffer* fb = (framebuffer*)0x100038;
    int width = (int)(fb->width);
    fb->address[x + width*y] = color;
}

void put_char(uint32_t foreground, uint32_t background, char character) {
    textmode *tm = (textmode*)0x100030;
    for (int i = 1; i <= 8; i++) {
        for (int j = 0; j < 16; j++) {
            char* bitmap = (ascii_chars + character)[0];
            int bit = (bitmap[j] >> (8-i)) & 0x1;
            if (bit) {
                put_pixel(tm->x_position*8 + i, j + tm->y_position*16, foreground);
            } else {
                put_pixel(tm->x_position*8 + i, j + tm->y_position*16, background);
            }
        }
    }
    if (tm->x_position == tm->width) {
        tm->y_position++;
        tm->x_position = 0;
    } else {
        tm->x_position++;
    }
}
void fill_screen(uint32_t color) {
    framebuffer* fb = (framebuffer *)0x100038;
    for (int x = 0; x < fb->width; x++) {
        for (int y = 0; y < fb->height; y++) {
            put_pixel(x, y, color);
        }
    }
}

void printf(char* string ,uint32_t foreground, uint32_t background) {
    textmode *tm = (textmode*)0x100030;
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] == '\n') {
            tm->y_position++;
            tm->x_position = 0;
        } else {
            put_char(foreground, background, string[i]);
        }
        i++;
    }
}
void printhex(uint64_t integer, uint32_t foreground, uint32_t background) {
    printf("0x", foreground, background);
    for (int i = 15; i >= 0; i--) {
        int val = (int)((integer >> i*4) & 0xf);
        if (val <= 9) {
            put_char(foreground, background, (char)(val + 48));
        } else {
            put_char(foreground, background, (char)(val + 87));
        }
    }
}

void init_text_mode(BootInformationFormat* multiboot_structure) {
    textmode *tm = (textmode*)0x100030;
    tm->width = (uint16_t)multiboot_structure->framebuffer_width/8;
    tm->height = (uint16_t)multiboot_structure->framebuffer_height/16;
    tm->x_position = 0;
    tm->y_position = 0;
}

void init_framebuffer(BootInformationFormat* multiboot_strucure) {
    framebuffer *fb = (framebuffer*)0x100038;
    fb->address = (uint32_t*)multiboot_strucure->framebuffer_addr;
    fb->width = multiboot_strucure->framebuffer_width;
    fb->height = multiboot_strucure->framebuffer_height;
    fb->pitch = multiboot_strucure->framebuffer_pitch;
}