#ifndef PAGING_H
#define PAGING_H
#include "../../../src/include/uint.h"
#include "../../../src/include/bootinfo.h"


#define PAGE_SIZE          (uint64_t)0x200000
#define NPAGES                  (int)0x40000
#define NP2_TABLES              (int)0x200
#define NP3_TABLES              (int)0x1

#define P2_ENTRIES         (uint64_t)0x200

//paging.asm
extern uint64_t p4_table[];
extern uint64_t p3_table[];
extern uint64_t p2_table[];
extern uint64_t page_stack_bottom[];
extern uint64_t page_stack_ptr[];       //this is no pointer instead an index of page_stack_bottom


typedef struct __attribute__((packed)) MemoryInformation {
    uint64_t memory_size;
} MemoryInformation;

#define HINT_BUFFER_SIZE    63
typedef struct __attribute__((packed, aligned(64))) PageAllocatorHint {
    int buffer[HINT_BUFFER_SIZE];
    int index;
} PageAllocatorHint;

extern int fallback_index;

extern PageAllocatorHint page_hint;

void init_mem(MemoryMap* mmap);
uint64_t kmalloc(int size, int read_write, int user_supervisor, int write_through, int cache_disable);
uint64_t kmalloc_phys_page(int read_write, int user_supervisor, int write_through, int cache_disable);
void kmfree(uint64_t base_addr, int size);
uint64_t map_vmem_to_pmem(uint64_t base_addr, int size, int read_write, int user_supervisor, int write_through, int cache_disable);
int identity_map(uint64_t base_addr, int size, int read_write, int user_supervisor, int write_through, int cache_disable);
void io_map(uint64_t base_addr, int size, int read_write, int user_supervisor, int write_through, int cache_disable);

#endif