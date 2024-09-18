#ifndef IO_H
#define IO_H
#include "../../src/include/uint.h"

static inline void outb(uint16_t port, uint8_t val){
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0"
                   : "=a"(ret)
                   : "Nd"(port)
                   : "memory");
    return ret;
}

static inline void io_wait(void){
    outb(0x80, 0);
}

void write_char_to_serial(char character);
void write_string_to_serial(char* string);
void write_hex_to_serial(uint64_t integer);
#endif