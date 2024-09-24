#ifndef CPAGING_H
#define CPAGING_H
#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"

#define PAGE_SIZE               (uint64_t)0x200000
#define NPAGES                  (int)0x2000

typedef struct __attribute__((packed)) MemoryInformation {
    uint64_t memory_size;
} MemoryInformation;

void init_mem(MemoryMap* mmap);
uint64_t kmalloc(int size, int read_write, int user_supervisor, int write_through, int cache_disable);
void dump_vmem(void);
uint64_t map_vmem_to_pmem(uint64_t base_addr, int size, int read_write, int user_supervisor, int write_through, int cache_disable);
int identity_map(uint64_t base_addr, int size, int read_write, int user_supervisor, int write_through, int cache_disable);
void io_map(uint64_t base_addr, int size, int read_write, int user_supervisor, int write_through, int cache_disable);
void dump_p2(void);
#endif