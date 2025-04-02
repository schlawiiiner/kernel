#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include "../../src/include/uint.h"
#include "../../src/include/pci.h"


#define CODE_SEG     0x0008
#define DATA_SEG     0x0010


typedef struct __attribute__((packed)) GateDescriptor {
    uint16_t offset_1;        // offset bits 0..15
    uint16_t selector;        // a code segment selector in GDT or LDT
    uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t  type_attributes; // gate type, dpl, and p fields
    uint16_t offset_2;        // offset bits 16..31
    uint32_t offset_3;        // offset bits 32..63
    uint32_t zero;            // reserved
} GateDescriptor;

#define IRQ_TOTAL_COUNT                 0x100
#define IRQ_EXCEPTION_COUNT             0x21

/*IRQ_MAP.type*/
#define IRQ_UNUSED                      0x00
#define IRQ_EXCEPTION                   0x01
#define IRQ_HARDWARE                    0x02
#define IRQ_SOFTWARE                    0x03

typedef struct __attribute__((packed)) IRQ_Map {
    uint8_t type;
    uint8_t reserved[7];
    volatile PCI_DEV* device;
} IRQ_Map;

typedef void (*func_ptr_t)(uint64_t*, uint64_t);

extern func_ptr_t irq_handlers[IRQ_TOTAL_COUNT];

void default_handler_func(uint64_t* rsp, uint64_t irq);
uint8_t map_isr(func_ptr_t function, volatile PCI_DEV* device);
void init_irq_map();
#endif