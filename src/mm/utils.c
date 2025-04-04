#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/io.h"

void __attribute__((optimize("O0"))) memset(uint64_t base_addr, uint64_t value, uint32_t size) {
    for (int i = 0; i < size/8; i++) {
        ((uint64_t*)base_addr)[i] = value;
    }
}

void memcopy(uint64_t* source, uint64_t* destination, int size) {
    if (size % 8) {
        print("ERROR: size is not multiple of 8 byte");
        while(1);
    }
    for (int i = 0; i < size/8; i++) {
        destination[i] = source[i];
    }
}

int compare_string(char* str1, char* str2) {
    int i = 0;
    while(str1[i] == str2[i]) {
        if (str1[i] == '\0') {
            return 0x1;
        }
        i++;
    }
    return 0x0;
}