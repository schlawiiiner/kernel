#include "../../../src/include/uint.h"
typedef struct Slab Slab;

struct __attribute__((aligned(64))) Slab {
    uint64_t base_addr;
    uint64_t chunk_size;
    int slab_size;
    int stack_ptr;
    uint32_t *stack;    // stores the offset within base_addr
    Slab* previous;
    Slab* next;
};

#define SLAB_STRUCT_POOL_SIZE       56
typedef struct SlabStructPool SlabStructPool;
struct __attribute__((packed, aligned(0x1000))) SlabStructPool {
    int stack_ptr;
    int reserved2[3];
    SlabStructPool *previous;
    SlabStructPool *next;
    uint64_t base_addr;
    uint64_t stack[SLAB_STRUCT_POOL_SIZE];
    uint64_t reserved3[3];
    Slab pool[SLAB_STRUCT_POOL_SIZE];
};

void allocate_new_slab(uint64_t chunk_size, uint32_t size);
void init_slab_allocator();