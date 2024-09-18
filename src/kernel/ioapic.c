#include "../../src/include/uint.h"
#include "../../src/include/cpaging.h"
#include "../../src/include/graphics.h"

void init_IOAPIC(uint32_t* IOAPIC_base) {
    identity_map((uint64_t)IOAPIC_base, 1);
    int number_of_redirection_entries = (IOAPIC_base[1] >> 16) & 0xff;
    printhex(number_of_redirection_entries);
}