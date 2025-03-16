#ifndef MP_H
#define MP_H
#include "../../src/include/uint.h"
#include "../../src/include/gdt.h"

typedef struct __attribute__((packed)) CPU {
    uint8_t Processor_ID;
    uint8_t APIC_ID;
    uint8_t Initialized;
    uint8_t reserved[5];
    TaskStateSegment Tss;
} CPU;

typedef struct __attribute__((packed)) CPUs {
    uint8_t number;
    uint8_t reserved[7];
    CPU cpu[];
} CPUs;

typedef struct Task Task;

struct __attribute__((packed)) Task {
    uint64_t ID;
    Task* previous;
    Task* next;
};

typedef struct __attribute__((packed)) TaskQueue {
    int size;
    volatile int lock;
    uint64_t task_counter;
    Task* enqueue;
    Task* dequeue;
} TaskQueue;

void init_cpus(void);
void switch_cpu(int cpu_number);
extern volatile CPUs* cpus;
#endif