#include "../../src/include/uint.h"
#include "../../src/include/cpaging.h"
#include "../../src/include/graphics.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/apic.h"

//memory indexed read
uint32_t __attribute__((optimize("O0"))) read_IOAPIC_reg(uint32_t* IOAPIC_base, uint8_t offset) {
    ((uint8_t*)IOAPIC_base)[0] = offset;
    return (IOAPIC_base + 4)[0];
}

//memory indexed write
void __attribute__((optimize("O0"))) write_IOAPIC_reg(uint32_t* IOAPIC_base, uint8_t offset, uint32_t val) {
    ((uint8_t*)IOAPIC_base)[0] = offset;
    (IOAPIC_base + 4)[0] = val;
}

void write_redirection_entry(uint32_t* IOAPIC_base, uint8_t irq, int vector, int delivery_mode, int destination_mode, int pin_polarity, int trigger_mode, int mask, int destination) {
    uint32_t low_bits = read_IOAPIC_reg(IOAPIC_base, 0x10 + irq*2);
    uint32_t high_bits = read_IOAPIC_reg(IOAPIC_base, irq*2+0x11);
    low_bits &= (1<<12) | (1<<14);
    high_bits &= 0xffffff;
    low_bits |= ((trigger_mode & 0b1) << 15) | ((pin_polarity & 0b1) << 13) | ((destination_mode & 0b1) << 11) | ((delivery_mode & 0b111) << 8) | (vector & 0xff);
    high_bits |= ((destination & 0xff) << 24) | (mask & 0b1);
    write_IOAPIC_reg(IOAPIC_base, 0x10 + irq*2, low_bits);
    write_IOAPIC_reg(IOAPIC_base, 0x11 + irq*2, high_bits);
}

void init_IOAPIC(uint32_t* IOAPIC_base) {
    //registers are memory indexed
    identity_map((uint64_t)IOAPIC_base, 1, 1, 0, 0, 0);
    uint32_t n_vectors = (read_IOAPIC_reg(IOAPIC_base, 0x1) >> 16) & 0xff;
    //timer
    write_redirection_entry(IOAPIC_base, 2 , 0x21, 0,0,0,0,0,0);
}