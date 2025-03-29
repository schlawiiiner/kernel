#ifndef INTERRUPTS_H
#define INTERRUPTS_H
#include "../../src/include/uint.h"

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

extern GateDescriptor IDT[256];

typedef void (*func_ptr_t)(uint64_t, uint64_t*);

extern func_ptr_t irq_handlers[256];

void default_handler_func(uint64_t irq, uint64_t* rsp);
void irq_handler(uint64_t irq ,uint64_t* rsp);
void map_isr(uint8_t irq, func_ptr_t function);
void init_default_handler();
uint8_t request_irq_for_mapping();
#endif