#include "../../src/include/mm/memory.h"
#include "../../src/include/io.h"

uint64_t allocate_physical_page() {
    uint32_t page_id = mem_info.page_stack[mem_info.stack_ptr];
    mem_info.stack_ptr--;
    if (mem_info.stack_ptr < 0) {
        write_string_to_serial("ERROR: out of memory");
        asm volatile ("hlt");
    }
    return (uint64_t)PAGE_SIZE_*page_id;
}

void free_physical_page(uint64_t addr) {
    uint32_t page_id = addr/PAGE_SIZE_;
    mem_info.stack_ptr++;
    mem_info.page_stack[mem_info.stack_ptr] = page_id;
}

void allocate_page_stack() {
    mem_info.memory_size = get_available_memory_size();
    uint64_t size = ((mem_info.memory_size/PAGE_SIZE_*sizeof(uint32_t))/PAGE_SIZE_+1)*PAGE_SIZE_;
    for (int i = 0; i < memory_map_size; i++) {
        if (memory_map[i].type == AVAILABLE) {
            if (memory_map[i].size > size) {
                insert_mmap_entry(memory_map[i].paddr, memory_map[i].paddr, PAGESTACK, size);
                mem_info.page_stack = (uint32_t*)memory_map[i].paddr;
                dump_mmap();
                return;
            }
        }
    }
    write_string_to_serial("no physical continuous space for page stack available");
    while(1);
}

void fill_page_stack() {
    memset((uint64_t)mem_info.page_stack, 0x0, mem_info.memory_size/PAGE_SIZE_*sizeof(uint32_t));
    mem_info.stack_ptr = -1;
    for (int j = 0; j < memory_map_size; j++) {
        if (memory_map[j].type == AVAILABLE) {
            uint32_t page_id = memory_map[j].paddr/PAGE_SIZE_;
            while (page_id < (memory_map[j].paddr+memory_map[j].size)/PAGE_SIZE_) {
                mem_info.stack_ptr++;
                mem_info.page_stack[mem_info.stack_ptr] = page_id;
                page_id++;
            }
        }
    }
}