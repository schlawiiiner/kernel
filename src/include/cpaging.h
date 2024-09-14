#ifndef CPAGING
#define CPAGING
#include "../../src/include/uint.h"

#define PAGE_SIZE               (uint64_t)0x200000
#define NPAGES                  (int)0x2000

void init_page_stack(void);
void map_virtual_memory_to_physical_memory(uint64_t virtual_addr, uint64_t physical_addr);
#endif