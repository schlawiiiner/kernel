#include "../include/mm/memory.h"
#include "../include/mm/slab.h"
#include "../include/graphics.h"
#include "../include/io.h"

#define SLAB_BUFFER_SIZE        11
Slab slab_buffer[SLAB_BUFFER_SIZE] __attribute__((section(".sysvar")));
SlabStructPool slab_struc_pool __attribute__((section(".sysvar")));

void allocate_new_slab_pool() {
    SlabStructPool* pool = (SlabStructPool*)kmalloc(PAGE_SIZE_);
    pool->previous = &slab_struc_pool;
    pool->next = slab_struc_pool.next;
    if (slab_struc_pool.next) {
        slab_struc_pool.next->previous = pool;
    }
    slab_struc_pool.next = pool;
    pool->base_addr = (uint64_t)pool;
    for (int i = 0; i < SLAB_STRUCT_POOL_SIZE; i++) {
        pool->stack[i] = (uint64_t)&pool->pool[i];
    }
}

// size is a multiple of the granularity
void allocate_new_slab(uint64_t chunk_size, uint32_t size) {
    int index;
    int granularity = 1024;
    if (size > 0x80000) {
        print("size is not supported by slab allocator");
        while(1);
    }
    switch (chunk_size) {
    case 0x4: index = 0; break;
    case 0x8: index = 1; break;
    case 0x10: index = 2; break;
    case 0x20: index = 3; break;
    case 0x40: index = 4; break;
    case 0x80: index = 5; break;
    case 0x100: index = 6; break;
    case 0x200: index = 7; break;
    case 0x400: index = 8; break;
    case 0x800: index = 9; break;
    case 0x1000: index = 10; break;
    default:
        print("chunk size is not supported by slab allocator");
        while(1);
        break;
    }
    Slab* slab;
    if (!slab_buffer[index].base_addr) {
        // here we allocate the first slab
        slab = &slab_buffer[index];
        slab->previous = 0x0;
        slab->next = 0x0;
    } else {
        // here we inserte a new slab to the linked list
        SlabStructPool *pool = &slab_struc_pool;
        while (1) {
            if ((pool->stack_ptr < SLAB_STRUCT_POOL_SIZE) && (pool->stack_ptr >= 0)) {
                slab = (Slab*)pool->stack[pool->stack_ptr];
                pool->stack_ptr--;
                slab->next = slab_buffer[index].next;
                slab->previous = &slab_buffer[index];
                if (slab->next) {
                    slab->next->previous = slab;
                }
                slab_buffer[index].next = slab;
                break;
            } else {
                if (pool->next) {
                    pool = pool->next;
                } else {
                    allocate_new_slab_pool();
                    pool = slab_struc_pool.next;
                }
            }
        }
    }
    uint64_t buffer = kmalloc(granularity*size*chunk_size);
    volatile uint32_t* stack = (volatile uint32_t*)kmalloc(granularity*size*sizeof(uint32_t));
    slab->base_addr = buffer;
    slab->stack = (uint32_t*)stack;

    slab->chunk_size = chunk_size;
    slab->slab_size = granularity*size;
    slab->stack_ptr = slab->slab_size - 1;
    if ((buffer == 0) || (stack == 0)) {
        // failed
    }
    for (volatile int i = 0; i < slab->slab_size; i++) {
        stack[i] = (uint32_t)(i*chunk_size);
    }
}
uint64_t malloc(uint64_t size) {
    int highestBit = 32 - __builtin_clz(size); 
    size = (uint64_t)1 << highestBit;
    int index = highestBit - 2;
    if (index < SLAB_BUFFER_SIZE) {
        Slab* slab = &slab_buffer[index];
        if (slab->slab_size == 0) {
            allocate_new_slab(size, 1);
        }
        while(1) {
            if ((slab->stack_ptr < slab->slab_size) && (slab->stack_ptr >= 0)) {
                uint64_t addr = slab->stack[slab->stack_ptr] + slab->base_addr;
                slab->stack_ptr--;
                return addr;
            } else {
                if (slab->next == 0x0) {
                    allocate_new_slab(size, 1);
                    slab = &slab_buffer[index];
                }
                slab = slab->next;
            }
        }
    } else {
        return kmalloc(size);
    }
}

void free(uint64_t addr, uint64_t size) {
    int highestBit = 32 - __builtin_clz(size); 
    size = (uint64_t)1 << (highestBit - 1);
    int index = highestBit - 3;
    if (index < SLAB_BUFFER_SIZE) {
        Slab* slab = &slab_buffer[index];
        if (slab->slab_size == 0) {
            print("ERROR: slab allocator was never initialized");
            while(1);
        }
        while(1) {
            if ((addr & ~(slab->base_addr)) < slab->slab_size*slab->chunk_size) {
                slab->stack_ptr++;
                if (slab->stack_ptr >= slab->slab_size) {
                    print("ERROR: Memory was freed multiple times");
                    while(1);
                }
                slab->stack[slab->stack_ptr] = (uint32_t)(addr & ~(slab->base_addr));
                return;
            } else {
                if (slab->next == 0x0) {
                    print("ERROR: slab allocator was never initialized");
                    while(1);
                }
                slab = slab->next;
            }
        }
    } else {
        kfree(addr, size);
    }
}

void init_slab_allocator() {
    slab_struc_pool.next = 0x0;
    slab_struc_pool.previous = 0x0;
    slab_struc_pool.base_addr = (uint64_t)&slab_struc_pool;
    slab_struc_pool.stack_ptr = -1;
    for (uint64_t addr = (uint64_t)&slab_struc_pool.pool; addr < ((uint64_t)&slab_struc_pool.pool + sizeof(Slab)*SLAB_STRUCT_POOL_SIZE); addr+=sizeof(Slab)) {
        slab_struc_pool.stack_ptr++;
        slab_struc_pool.stack[slab_struc_pool.stack_ptr] = addr;
    }
}