#ifndef MP_H
#define MP_H
#include "../../src/include/uint.h"
#include "../../src/include/gdt.h"

typedef struct __attribute__((packed)) CPU {
    uint8_t Processor_ID;
    uint8_t APIC_ID;
    uint8_t Initialized;
    uint8_t reserved[5];
    TSS tss;
} CPU;

typedef struct __attribute__((packed)) CPUs {
    uint8_t number;
    uint8_t reserved[7];
    CPU cpu[];
} CPUs;

#endif