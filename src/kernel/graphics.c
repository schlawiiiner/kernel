#include "../../src/include/font.h"
#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"
//#include "../../src/include/mm/paging.h"
#include "../../src/include/mm/memory.h"
#include "../../src/include/io.h"
#include "../../src/include/graphics.h"

volatile textmode tm __attribute__((section(".sysvar")));
volatile framebuffer fb __attribute__((section(".sysvar")));

void flush_cache_line(void *address) {
    // Ensure that the address is aligned to the cache line size
    // Typically, the cache line size is 64 bytes on modern CPUs
    __asm__ volatile (
        "clflush %0"
        : // No output
        : "m" (*(volatile uint64_t *)address) // Input: address to flush
        : "memory" // Clobber memory
    );
}

void set_lock() {
    flush_cache_line((void*)&tm);
    while (__sync_lock_test_and_set(&(tm.lock), 1));
    asm volatile("mfence" ::: "memory");
}

void release_lock() {
    asm volatile("mfence" ::: "memory");
    flush_cache_line((void*)&tm);
    __sync_lock_release(&tm.lock);
}


void put_pixel(int x, int y, uint32_t color) {
    int width = (int)(fb.width);
    fb.address[x + width*y] = color;
}

void fill_screen(uint32_t color) {
    for (int x = 0; x < fb.width; x++) {
        for (int y = 0; y < fb.height; y++) {
            put_pixel(x, y, color);
        }
    }
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
    if (tm.x_position == tm.width - 1) {
        if (tm.y_position == tm.height - 1) {
            fill_screen(tm.background);
            tm.x_position = 0;
            tm.y_position = 0;
        } else {
            tm.y_position++;
            tm.x_position = 0;
        }
    } else {
        tm.x_position++;
    }
}


void print(char* string) {
    set_lock();
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] == '\n') {
            if (tm.y_position == tm.height - 1) {
                fill_screen(tm.background);
                tm.y_position = 0;
                tm.x_position = 0;
            } else {
                tm.y_position++;
                tm.x_position = 0;
            }
        } else if (string[i] == '\t') {
            put_char(' ');
            put_char(' ');
            put_char(' ');
            put_char(' ');
        } else {
            put_char(string[i]);
        }
        i++;
    }
    release_lock();
}
void printhex(uint64_t integer) {
    set_lock();
    put_char('0');
    put_char('x');
    for (int i = 15; i >= 0; i--) {
        int val = (int)((integer >> i*4) & 0xf);
        if (val <= 9) {
            put_char((char)(val + 48));
        } else {
            put_char((char)(val + 87));
        }
    }
    release_lock();
}

void printdec(uint64_t integer) {
    set_lock();
    if (integer == 0) {
        put_char('0');  // Special case for zero
        release_lock();
        return;
    }

    char buffer[20]; // Buffer to store the digits (enough for 64-bit integers)
    int index = 0;

    // Convert the integer to a string in reverse order
    while (integer > 0) {
        int digit = integer % 10; // Get the last digit
        buffer[index++] = (char)(digit + '0'); // Store it as a character
        integer /= 10; // Remove the last digit
    }

    // Print the digits in the correct order
    for (int i = index - 1; i >= 0; i--) {
        put_char(buffer[i]);
    }
    release_lock();
}

void printbin(uint64_t integer) {
    set_lock();
    put_char('0');
    put_char('b');
    for (int i = 63; i >= 0; i--) {
        put_char((char)(((integer >> i) & 0x1) + 48));
    }
    release_lock();
}

void set_color(uint32_t foreground, uint32_t background) {
    tm.foreground = foreground;
    tm.background = background;
}

void set_cursor(uint32_t x, uint32_t y) {
    tm.x_position = x;
    tm.y_position = y;
}

void init_text_mode(FramebufferInfo* framebuffer_info) {
    tm.width = (uint16_t)framebuffer_info->framebuffer_width/8;
    tm.height = (uint16_t)framebuffer_info->framebuffer_height/16;
    tm.x_position = 0;
    tm.y_position = 0;
    tm.foreground = 0x00ff00;
    tm.background = 0x000000;
    fill_screen(0x000000);
    tm.lock = 0;
}

void init_framebuffer(FramebufferInfo* framebuffer_info) {
    fb.width = framebuffer_info->framebuffer_width;
    fb.height = framebuffer_info->framebuffer_height;
    fb.pitch = framebuffer_info->framebuffer_pitch;
    fb.address = (uint32_t*)mmap(framebuffer_info->framebuffer_addr, fb.height*fb.pitch);
}

void init_graphics(FramebufferInfo* framebuffer_info) {
    init_framebuffer(framebuffer_info);
    init_text_mode(framebuffer_info);
}