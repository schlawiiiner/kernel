#include "../../src/include/font.h"
#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/cpaging.h"
#include "../../src/include/io.h"

struct __attribute__((packed)) textmode {
    uint64_t width;
    uint64_t height;
    uint64_t x_position;
    uint64_t y_position;
    uint64_t foreground;
    uint64_t background;
};

typedef struct textmode textmode;

struct __attribute__((packed)) framebuffer {
    uint32_t* address;
    uint64_t pitch;
    uint64_t width;
    uint64_t height;
};

typedef struct framebuffer framebuffer;

//sysvar.asm
extern framebuffer fb;
extern textmode tm;


void put_pixel(int x, int y, uint32_t color) {
    int width = (int)(fb.width);
    fb.address[x + width*y] = color;
}

void put_char(char character) {
    for (int i = 1; i <= 8; i++) {
        for (int j = 0; j < 16; j++) {
            char* bitmap = (ascii_chars + character)[0];
            int bit = (bitmap[j] >> (8-i)) & 0x1;
            if (bit) {
                put_pixel(tm.x_position*8 + i, j + tm.y_position*16, tm.foreground);
            } else {
                put_pixel(tm.x_position*8 + i, j + tm.y_position*16, tm.background);
            }
        }
    }
    if (tm.x_position == tm.width) {
        tm.y_position++;
        tm.x_position = 0;
    } else {
        tm.x_position++;
    }
}
void fill_screen(uint32_t color) {
    for (int x = 0; x < fb.width; x++) {
        for (int y = 0; y < fb.height; y++) {
            put_pixel(x, y, color);
        }
    }
}

void printf(char* string) {
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] == '\n') {
            tm.y_position++;
            tm.x_position = 0;
        } else {
            put_char(string[i]);
        }
        i++;
    }
}
void printhex(uint64_t integer) {
    printf("0x");
    for (int i = 15; i >= 0; i--) {
        int val = (int)((integer >> i*4) & 0xf);
        if (val <= 9) {
            put_char((char)(val + 48));
        } else {
            put_char((char)(val + 87));
        }
    }
}

void printbin(uint64_t integer) {
    printf("0b");
    for (int i = 63; i >= 0; i--) {
        put_char((char)(((integer >> i) & 0x1) + 48));
    }
}

void set_color(uint32_t foreground, uint32_t background) {
    tm.foreground = foreground;
    tm.background = background;
}

void init_text_mode(FramebufferInfo* multiboot_structure) {
    tm.width = (uint16_t)multiboot_structure->framebuffer_width/8;
    tm.height = (uint16_t)multiboot_structure->framebuffer_height/16;
    tm.x_position = 0;
    tm.y_position = 0;
    tm.foreground = 0x00ff00;
    tm.background = 0x000000;
    fill_screen(0x000000);
}

void init_framebuffer(FramebufferInfo* multiboot_strucure) {
    fb.width = multiboot_strucure->framebuffer_width;
    fb.height = multiboot_strucure->framebuffer_height;
    fb.pitch = multiboot_strucure->framebuffer_pitch;

    uint64_t n_pages_for_fb = (fb.height*fb.pitch) / PAGE_SIZE;
    if (((fb.height*fb.pitch) % PAGE_SIZE) != 0) {
        n_pages_for_fb += 1; 
    }
    write_string_to_serial("\nFramebuffer Address: ");
    write_hex_to_serial(multiboot_strucure->framebuffer_addr);
    write_string_to_serial("\n");
    fb.address = (uint32_t*)map_vmem_to_pmem(multiboot_strucure->framebuffer_addr, n_pages_for_fb);
}