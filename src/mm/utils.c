#include "../../src/include/uint.h"
#include "../../src/include/mm/paging.h"
#include "../../src/include/graphics.h"
#include "../../src/include/io.h"

void dump_vmem(int n) {
    for (int i = 0; i < 0x200*n; i++) {
        if (p2_table[i] & 0x1) {
            print("1");
        } else {
            print("0");
        }
    }
    print("\n");
}

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

void dump_p2() {
    for (int i = 0; i < NPAGES; i++) {
        if (p2_table[i] & 0x1) {
            write_hex_to_serial((i*(uint64_t)0x200000));
            write_string_to_serial(": ");
            write_hex_to_serial(p2_table[i]);
            write_string_to_serial("\n");
        }
    }
}