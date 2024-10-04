#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include "../../../src/include/uint.h"

#ifndef MAX_RAMSIZE
    #define MAX_RAMSIZE             (uint64_t)0x400000000
#endif

#define ROUND_UP(x, y)              (((x) + (y) - 1) / (y))

#define N_ALLOCATORS                15
#define N_PHYSICAL_PAGES            ROUND_UP(MAX_RAMSIZE, PAGE_SIZE)
#define ALLOCATOR_OFFSET            N_PHYSICAL_PAGES*sizeof(uint64_t)
#define N_POINTER_PAGES             ROUND_UP(ALLOCATOR_OFFSET*N_ALLOCATORS, PAGE_SIZE)

/*
Here we implement a stack based slab allocator, while providing effective hints
*/

// this struct fits exactly in one cache line
typedef struct __attribute__((packed, aligned(64))) Allocator {
    int chunk_size;
    int busy;
    int r1;
    int r2;
    uint64_t hint;
    uint64_t** stack_ptr_to_hint;
    uint64_t** stack_ptr_table_base;
    int stack_size;
    int stacks_per_page;
    uint64_t r3;
    uint64_t r4;
} Allocator;


uint64_t malloc(int size);
void free(uint64_t addr);
void init_allocators();
#endif