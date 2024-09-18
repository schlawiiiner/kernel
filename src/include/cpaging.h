#ifndef CPAGING
#define CPAGING
#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"

#define PAGE_SIZE               (uint64_t)0x200000
#define NPAGES                  (int)0x2000

typedef struct __attribute__((packed)) MemoryInformation {
    uint64_t memory_size;
} MemoryInformation;

void init_mem(MemoryMap* mmap);
uint64_t vmalloc(int size);
void dump_vmem(void);
uint64_t map_vmem_to_pmem(uint64_t base_addr, int size);
int identity_map(uint64_t base_addr, int size);
void dump_p2(void);
#endif