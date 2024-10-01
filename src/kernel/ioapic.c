#include "../../src/include/uint.h"
#include "../../src/include/cpaging.h"
#include "../../src/include/graphics.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/apic.h"
#include "../../src/include/ioapic.h"
#include "../../src/include/interrupts.h"

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
    low_bits |= ((mask & 0b1)<< 16) | ((trigger_mode & 0b1) << 15) | ((pin_polarity & 0b1) << 13) | ((destination_mode & 0b1) << 11) | ((delivery_mode & 0b111) << 8) | (vector & 0xff);
    high_bits |= ((destination & 0xff) << 24);
    write_IOAPIC_reg(IOAPIC_base, 0x10 + irq*2, low_bits);
    write_IOAPIC_reg(IOAPIC_base, 0x11 + irq*2, high_bits);
}

uint64_t read_redirection_entry(uint32_t* IOAPIC_base, uint8_t irq) {
    return ((uint64_t)read_IOAPIC_reg(IOAPIC_base, 0x11 + irq*2) << 32) | read_IOAPIC_reg(IOAPIC_base, 0x10 + irq*2);
}

uint8_t max_redirection_entries(uint32_t* IOAPIC_base) {
    return (uint8_t)((read_IOAPIC_reg(IOAPIC_base, 0x1) >> 16) & 0xff) + 1;
}

void activate_pins() {
    int n_ioapics = ioapic_list.number;
    irq_probe = 0;
    for (int ioapic = 0; ioapic < n_ioapics; ioapic++) {
        int base = ioapic_list.ioapics[ioapic].InterruptBase;
        int max = max_redirection_entries(ioapic_list.ioapics[ioapic].IOAPICBase) + base;
        for (int gsi = base; gsi < max; gsi++) {
            if (!ioapic_list.pins[gsi]) {
                write_redirection_entry(ioapic_list.ioapics[ioapic].IOAPICBase, gsi, 32+gsi, 0, 0, 0, 0, 0, 0);
                
            }
        }
    }
}

void deactivate_pins() {
    int n_ioapics = ioapic_list.number;
    for (int ioapic = 0; ioapic < n_ioapics; ioapic++) {
        int base = ioapic_list.ioapics[ioapic].InterruptBase;
        int max = max_redirection_entries(ioapic_list.ioapics[ioapic].IOAPICBase) + base;
        for (int gsi = base; gsi < max; gsi++) {
            write_redirection_entry(ioapic_list.ioapics[ioapic].IOAPICBase, gsi, 32+gsi, 0, 0, 0, 0, 1, 0);
        }
    }
}

int find_pins() {
    int timeout = 0x20000000;
    while(!irq_probe) {
        if (timeout == 0) {
            return -1;
        }
        timeout--;
    }
    return irq_probe-32;
}

void map_IOAPIC_MMIO(MADT_IO_APIC* ioapic) {
    int id = (int)ioapic_list.number;
    ioapic_list.ioapics[id].ID = ioapic->IO_APIC_ID;
    ioapic_list.ioapics[id].InterruptBase = ioapic->Global_System_Interrupt_Base;
    ioapic_list.ioapics[id].IOAPICBase = (uint32_t*)((uint64_t)(ioapic->IO_APIC_Address));
    uint32_t* IOAPIC_base = ioapic_list.ioapics[id].IOAPICBase;
    ioapic_list.number++;
    identity_map((uint64_t)IOAPIC_base, 1, 1, 0, 0, 0);
}

void init_IOAPIC() {
    while (1) {
        activate_pins();
        int gsi = find_pins();
        if (gsi > 0) {
            ioapic_list.pins[gsi] = 1;
        } else {
            deactivate_pins();
            return;
        }
        deactivate_pins();
    }
}

void route_hardware_interrupt(int irq, int gsi, func_ptr_t isr) {
    for (int i = 0; i < ioapic_list.number; i++) {
        int base = ioapic_list.ioapics[i].InterruptBase;
        int max = max_redirection_entries(ioapic_list.ioapics[i].IOAPICBase) + base;
        if (base <= gsi < max) {
            irq_handlers[irq] = isr;
            write_redirection_entry(ioapic_list.ioapics[i].IOAPICBase, gsi - base, irq, 0, 0, 0, 0, 0, 0);
            printbin(read_redirection_entry(ioapic_list.ioapics[i].IOAPICBase, gsi - base));
            printf("\n");
            return;
        }
    }
    printf("ERROR");
    while(1);
}