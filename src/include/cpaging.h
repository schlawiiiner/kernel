#ifndef CPAGING
#define CPAGING
#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"

#define PAGE_SIZE               (uint64_t)0x200000
#define NPAGES                  (int)0x2000

void init_page_table(void);
void init_page_stack(MemoryMap* mmap);
void map_virtual_memory_to_physical_memory(uint64_t virtual_addr, uint64_t physical_addr);
uint64_t vmalloc(int size);
void dump_vmem(void);
#endif