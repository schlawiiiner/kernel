#ifndef GRAPHICS
#define GRAPHICS
#include "../../src/include/uint.h"
#include "../../src/include/font.h"
#include "../../src/include/bootinfo.h"

#define TEXTMODE 0x100030
#define FRAMEBUFFER 0x100060

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
    uint64_t foreground;
    uint64_t background;
};

typedef struct framebuffer framebuffer;

void put_pixel(int x, int y, uint32_t color);
void put_char(char character);
void fill_screen(uint32_t color);
void printf(char* string);
void printhex(uint64_t integer);
void printbin(uint64_t integer);
void set_color(uint32_t foreground, uint32_t background);
void init_text_mode(FramebufferInfo* multiboot_structure);
void init_framebuffer(FramebufferInfo* multiboot_strucure);
#endif