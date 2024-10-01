#ifndef IOAPIC_H
#define IOAPIC_H
#include "../../src/include/uint.h"
#include "../../src/include/acpi.h"
#include "../../src/include/interrupts.h"

typedef struct __attribute__((packed)) IOAPIC {
    uint8_t ID;
    uint8_t reserved[3];
    uint32_t InterruptBase;
    uint32_t* IOAPICBase;
} IOAPIC;

typedef struct __attribute__((packed)) IOAPIC_List {
    uint32_t number;
    uint32_t number_gsi;
    uint8_t pins[96];
    IOAPIC ioapics[4];
} IOAPIC_List;


extern volatile uint64_t irq_probe;
extern volatile IOAPIC_List ioapic_list;

uint64_t read_redirection_entry(uint32_t* IOAPIC_base, uint8_t irq);

void map_IOAPIC_MMIO(MADT_IO_APIC* ioapic);
void init_IOAPIC();
int find_pins();
void activate_pins();
void deactivate_pins();
void route_hardware_interrupt(int irq, int gsi, func_ptr_t isr);
#endif