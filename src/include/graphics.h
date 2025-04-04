#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "../../src/include/uint.h"
#include "../../src/include/font.h"
#include "../../src/include/bootinfo.h"

typedef struct __attribute__((packed, aligned(64))) textmode {
    uint64_t width;
    uint64_t height;
    uint64_t x_position;
    uint64_t y_position;
    uint64_t foreground;
    uint64_t background;
    uint64_t lock;
} textmode;

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
void put_chars(char* string, int n);
void fill_screen(uint32_t color);
void print(char* string);
void printhex(uint64_t integer);
void printdec(uint64_t integer);
void printbin(uint64_t integer);
void set_color(uint32_t foreground, uint32_t background);
void set_cursor(uint32_t x, uint32_t y);
void init_graphics(void);
#endif