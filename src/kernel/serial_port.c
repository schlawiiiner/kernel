#include "../../src/include/io.h"

void write_char_to_serial(char character) {
    outb(0x3f8, character);
}

void write_string_to_serial(char* string) {
    int i = 0;
    while (string[i] != '\0') {
        write_char_to_serial(string[i]);
        i++;
    }
}

void write_hex_to_serial(uint64_t integer) {
    write_string_to_serial("0x");
    for (int i = 15; i >= 0; i--) {
        int val = (int)((integer >> i*4) & 0xf);
        if (val <= 9) {
            write_char_to_serial((char)(val + 48));
        } else {
            write_char_to_serial((val + 87));
        }
    }
}