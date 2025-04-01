#ifndef MP_H
#define MP_H
#include "../../src/include/uint.h"
#include "../../src/include/gdt.h"

typedef struct __attribute__((packed)) Registers {
    uint64_t RAX;
    uint64_t RBX;
    uint64_t RCX;
    uint64_t RDX;
    uint64_t RSI;
    uint64_t RDI;
    uint64_t RBP;
    uint64_t RSP;

    uint64_t RIP;

    uint64_t R8;
    uint64_t R9;
    uint64_t R10;
    uint64_t R11;
    uint64_t R12;
    uint64_t R13;
    uint64_t R14;
    uint64_t R15;

    uint64_t CR3;

    uint64_t RFLAGS;
    
    uint64_t XMM[16];
    uint64_t YMM[16];

    uint16_t CS;
    uint16_t DS;
    uint16_t ES;
    uint16_t FS;
    uint16_t GS;
    uint16_t SS;
} Registers;
typedef struct Task Task;

struct __attribute__((packed)) Task {
    uint64_t ID;
    Task* previous;
    Task* next;
    Registers registers;
};

typedef struct __attribute__((packed)) TaskQueue {
    int size;
    volatile int lock;
    uint64_t task_counter;
    Task* enqueue;
    Task* dequeue;
} TaskQueue;

typedef struct __attribute__((packed)) CPU {
    uint8_t Processor_ID;
    uint8_t APIC_ID;
    uint8_t Initialized;
    uint8_t reserved[5];
    Task* Current_Task;
    TaskStateSegment Tss;
} CPU;

typedef struct __attribute__((packed)) CPUs {
    uint8_t number;
    uint8_t reserved[7];
    CPU cpu[];
} CPUs;

static inline void cpuid(int code, int subleaf, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
    asm volatile("cpuid"
                 : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                 : "a"(code), "c"(subleaf));
}

void init_cpus(void);
void switch_cpu(int cpu_number);
void assign_per_core_struct(uint32_t apic_id);
void enable_cpu_features(void);
extern volatile CPUs* cpus;
#endif