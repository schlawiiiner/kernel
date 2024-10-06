#include "../../src/include/uint.h"
#include "../../src/include/mm/allocator.h"
#include "../../src/include/mm/paging.h"
#include "../../src/include/mm/utils.h"
#include "../../src/include/io.h"
#include "../../src/include/graphics.h"

uint64_t** pointer_table;
Allocator allocators[N_ALLOCATORS] __attribute__((aligned(64)));
int* page_table_free;

int find_allocator(int size) {
    int i = 0;
    while (size >> (i+7)){
        i++;
    }
    if (size > (1 << (i+6))) {
        i++;
    }
    return i;
}

void allocate_new_pool(Allocator* allocator, int index) {
    for (int i = 0; i < N_PHYSICAL_PAGES; i++) {
        if (allocator->stack_ptr_table_base[i] == 0x0) {
            // allocate a new pool
            uint64_t pool = kmalloc(1, 1, 0 ,0, 0);
            page_table_free[2*(int)(pool/PAGE_SIZE)] = index;
            page_table_free[2*(int)(pool/PAGE_SIZE)+1] = i;
            uint64_t* stack;
            int offset = i % (allocator->stacks_per_page);
            if (!offset) {
                int present = 0;
                for (int j = i+1; j < i + allocator->stacks_per_page; j++) {
                    if (allocator->stack_ptr_table_base[j] != 0x0) {
                        stack = (uint64_t*)(((uint64_t)allocator->stack_ptr_table_base[j]/PAGE_SIZE)*PAGE_SIZE);
                        present = 1;
                        break;
                    }
                }
                if (!present) {
                    stack = (uint64_t*)kmalloc(1, 1, 0, 0, 0);
                }
            } else {
                stack = (uint64_t*)(((uint64_t)allocator->stack_ptr_table_base[i-1]/PAGE_SIZE)*PAGE_SIZE + offset*allocator->stack_size*sizeof(uint64_t));
            }
            // fill the stack
            for (int j = 0; j < allocator->stack_size; j++) {
                stack[j] = pool;
                pool+=allocator->chunk_size;
            }
            //set the stack pointer
            allocator->stack_ptr_table_base[i] = stack + allocator->stack_size;
            //generate a new hint
            allocator->stack_ptr_to_hint = &(allocator->stack_ptr_table_base[i]);
            allocator->hint = allocator->stack_ptr_table_base[i][-1];
            return;
        }
    }
    print("ERROR: allocation of new memory pool failed: out of memory");
    while(1);
}

void deallocate_pool(Allocator* allocator, int page_table_free_index, int stack_index) {
    kmfree(page_table_free_index/2*PAGE_SIZE, 1);
    uint64_t stack_base = (uint64_t)allocator->stack_ptr_table_base[stack_index]-allocator->stack_size*sizeof(uint64_t);
    
   allocator->stack_ptr_table_base[stack_index] = 0x0;

    page_table_free[page_table_free_index] = 0xffffffff;
    page_table_free[page_table_free_index+1] = 0xffffffff;

    int stack_offset = stack_index % allocator->stacks_per_page;
    for (int i = stack_index - stack_offset; i < stack_index - stack_offset + allocator->stacks_per_page; i++) {
        if (allocator->stack_ptr_table_base[i] != 0x0) {
            // page is beeing used for other stacks
            return;
        }
    }
    // page can be freed
    kmfree((stack_base/PAGE_SIZE)*PAGE_SIZE, 1);
}

void generate_new_hint(Allocator* allocator) {
    for (int i = 0; i < N_PHYSICAL_PAGES; i++) {
        if (allocator->stack_ptr_table_base[i] != 0x0) {
            // found a present pool, now we check if it is exhausted
            if ((uint64_t)allocator->stack_ptr_table_base[i] & (allocator->stack_size*8 - 1)) {
                // found a non-empty pool
                allocator->stack_ptr_to_hint = &(allocator->stack_ptr_table_base[i]);
                allocator->hint = allocator->stack_ptr_to_hint[-1][0];
                return;
            } else {
                // pool is exhausted, we continue searching for a non-empty pool ...
                continue;
            }
        }
    }
    // we failed to generate a new hint
    allocator->hint = 0x0;
    allocator->stack_ptr_to_hint = 0x0;
    return;
}


uint64_t malloc(int size) {
    if (size >= PAGE_SIZE) {
        if (size % PAGE_SIZE) {
            uint64_t addr =  kmalloc(size/PAGE_SIZE+1, 1, 0, 0, 0);
            page_table_free[2*addr/PAGE_SIZE] = N_ALLOCATORS;
            page_table_free[2*addr/PAGE_SIZE+1] = size/PAGE_SIZE+1;
            return addr;
        } else {
            uint64_t addr =  kmalloc(size/PAGE_SIZE, 1, 0, 0, 0);
            page_table_free[2*addr/PAGE_SIZE] = N_ALLOCATORS;
            page_table_free[2*addr/PAGE_SIZE+1] = size/PAGE_SIZE;
            return addr;
        }
    }

    int i = find_allocator(size);
    Allocator* allocator = &allocators[i];
    int expected = 0;
    while (1) {
        if (__atomic_compare_exchange_n(&(allocator->busy), &expected, 1, 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
            break;
        }
    }

    if (__builtin_expect(!allocator->hint, 0)) {
        generate_new_hint(allocator);
        
    }
    if (__builtin_expect(!allocator->hint, 0)) {
        // no hint is provided, this means all pools are exhausted
        allocate_new_pool(allocator, i);
    }

    uint64_t addr = allocator->hint;
    allocator->stack_ptr_to_hint[0]--;
    if (__builtin_expect((uint64_t)allocator->stack_ptr_to_hint[0] & (allocator->stack_size*8 - 1), 1)) {
        // since the pool is not exhausted we can provide a very fast hint
        allocator->hint = allocator->stack_ptr_to_hint[0][-1];
    } else {
        // pool now is exhausted, we now clear the hint leave this job to the next malloc call, since if a new chunk is freed
        // a new hint will be provided anyway
        allocator->hint = 0x0;
        allocator->stack_ptr_to_hint = 0x0;
    }
    __atomic_store_n(&allocator->busy, 0, __ATOMIC_RELEASE);
    return addr;
}

void free(uint64_t addr) {
    int index = 2*(addr/PAGE_SIZE);
    if (page_table_free[index] == N_ALLOCATORS) {
        kmfree(addr, page_table_free[index+1]);
        page_table_free[index] = 0x0;
        page_table_free[index+1] = 0x0;
        return;
    }
    Allocator* allocator = &allocators[page_table_free[index]];
    int stack_index = page_table_free[index+1];
    int expected = 0;

    while (1) {
        if (__atomic_compare_exchange_n(&(allocator->busy), &expected, 1, 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
            break;
        }
    }
    allocator->stack_ptr_table_base[stack_index][0] = addr;
    allocator->stack_ptr_table_base[stack_index]++;
    if ((uint64_t)allocator->stack_ptr_table_base[stack_index] % allocator->stack_size*sizeof(uint64_t)) {
        allocator->hint = addr;
        allocator->stack_ptr_to_hint = &allocator->stack_ptr_table_base[stack_index];
    } else {
        // stack is full, deallocate pool and clear the hint
        deallocate_pool(allocator, index, stack_index);
        allocator->hint = 0x0;
        allocator->stack_ptr_to_hint = 0x0;
    }
    __atomic_store_n(&allocator->busy, 0, __ATOMIC_RELEASE);
}

void dump_page_table_free(void) {
    for (int i = 0; i < PAGE_SIZE/4; i+=2){
        if (page_table_free[i] < 0) {
            continue;
        } else {
            write_string_to_serial("Address: ");
            write_hex_to_serial(i*PAGE_SIZE);
            write_string_to_serial(" Allocator: ");
            write_hex_to_serial(page_table_free[i]);
            write_string_to_serial(" Stack: ");
            write_hex_to_serial(page_table_free[i+1]);
            write_string_to_serial("\n");
        }
    }
}


void init_allocators() {
    pointer_table = (uint64_t**)kmalloc(N_POINTER_PAGES, 1, 0, 0, 0);
    memset((uint64_t)pointer_table, 0x0, PAGE_SIZE);
    for (int i = 0; i < N_ALLOCATORS; i++) {
        // initialize some metadata
        allocators[i].busy = 0;
        allocators[i].chunk_size = (1 << (i+6));
        allocators[i].stack_ptr_table_base = (uint64_t**)((uint64_t)pointer_table+ALLOCATOR_OFFSET*i);
        allocators[i].stack_size = PAGE_SIZE/allocators[i].chunk_size;
        allocators[i].stacks_per_page = PAGE_SIZE/(8*allocators[i].stack_size);
        allocators[i].hint = 0x0;
        allocators[i].stack_ptr_to_hint = 0x0;
    }
    page_table_free = (int*)kmalloc(1, 1, 0, 0, 0);
    memset((uint64_t)page_table_free, 0xffffffffffffffff, PAGE_SIZE);
    
}