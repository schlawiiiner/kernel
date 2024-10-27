#include "../../src/include/bootinfo.h"
#include "../../src/include/uint.h"
#include "../../src/include/mm/memory.h"
#include "../../src/include/io.h"

volatile MemoryInformation mem_info;
#define MEMORY_MAP_ENTRIES          100
TotalMemoryMap memory_map[MEMORY_MAP_ENTRIES];
int memory_map_size = 0;
extern KernelMemoryMap kernel_mmap[3];

// This function does not treat the boundary condition correct
void insert_mmap_entry(uint64_t paddr, uint64_t vaddr, uint64_t type, uint64_t size) {
    //first we search the slot where to insert the mmap entry
    int slot;
    for (int i = 0; i < memory_map_size; i++) {
        if ((memory_map[i].paddr <= paddr) && (paddr < memory_map[i].paddr + memory_map[i].size)) {
            slot = i;
        } 
    }
    if (memory_map[slot].paddr == paddr) {
        memory_map_size++;
        for (int i = memory_map_size-1; i > slot; i--) {
            memory_map[i].paddr = memory_map[i-1].paddr;
            memory_map[i].vaddr = memory_map[i-1].vaddr;
            memory_map[i].size = memory_map[i-1].size;
            memory_map[i].type = memory_map[i-1].type;
        }
        memory_map[slot+1].paddr = paddr + size;
        memory_map[slot+1].vaddr = vaddr + size;
        memory_map[slot+1].size  = memory_map[slot].size + memory_map[slot].paddr - paddr - size;
        memory_map[slot+1].type  = memory_map[slot].type;

        memory_map[slot].paddr = paddr;
        memory_map[slot].vaddr = vaddr;
        memory_map[slot].size  = size;
        memory_map[slot].type  = type;
    } else {
        memory_map_size+=2;
        for (int i = memory_map_size-1; i > slot; i--) {
            memory_map[i].paddr = memory_map[i-2].paddr;
            memory_map[i].vaddr = memory_map[i-2].vaddr;
            memory_map[i].size = memory_map[i-2].size;
            memory_map[i].type = memory_map[i-2].type;
        }

        memory_map[slot+2].paddr = paddr + size;
        memory_map[slot+2].vaddr = memory_map[slot].vaddr + paddr - memory_map[slot].paddr + size;
        memory_map[slot+2].size  = memory_map[slot].size + memory_map[slot].paddr - paddr - size;
        memory_map[slot+2].type  = memory_map[slot].type;

        memory_map[slot].size = paddr - memory_map[slot].paddr;

        memory_map[slot+1].paddr = paddr;
        memory_map[slot+1].vaddr = vaddr;
        memory_map[slot+1].size  = size;
        memory_map[slot+1].type  = type;
    }
}

void dump_mmap() {
    write_string_to_serial("\n\nMEMORY MAP:\n\n");
    write_string_to_serial("physical address\tvirtual address\t\tsize\t\t\t\ttype\n");
    for (int i = 0; i < memory_map_size; i++) {
        write_hex_to_serial(memory_map[i].paddr);
        write_string_to_serial("\t");
        write_hex_to_serial(memory_map[i].vaddr);
        write_string_to_serial("\t");
        write_hex_to_serial(memory_map[i].size);
        write_string_to_serial("\t");
        write_hex_to_serial(memory_map[i].type);
        write_string_to_serial("\n");
    }
}

void generate_mmap(BootInformation* multiboot_structure) {
    MemoryMapEntry * memory_map_entry = (MemoryMapEntry*)((uint64_t)bis.memory_map + 16);
    memory_map_size = (bis.memory_map->size - 16)/bis.memory_map->entry_size;

    // we copy GRUB's mmap to the systems memory map
    for (int i = 0; i < memory_map_size; i++) {
        memory_map[i].paddr = memory_map_entry[i].base_addr;
        memory_map[i].vaddr = memory_map_entry[i].base_addr;
        memory_map[i].size = memory_map_entry[i].length;
        memory_map[i].type = memory_map_entry[i].type;
    }
    // insert the KERNEL mmap entries
    for (int i = 0; i < 3; i++) {
        insert_mmap_entry(kernel_mmap[i].base_addr, kernel_mmap[i].virtual_base_addr, KERNEL, kernel_mmap[i].length);
    }
    // insert the BOOTINFO mmap entry
    insert_mmap_entry((uint64_t)multiboot_structure, (uint64_t)multiboot_structure, BOOTINFO, ((multiboot_structure->total_size)/PAGE_SIZE_+1)*PAGE_SIZE_);
}

uint64_t get_available_memory_size() {
    uint64_t size = 0;
    for (int i = 0; i < memory_map_size; i++) {
        if (memory_map[i].type == AVAILABLE) {
            size+=memory_map[i].size;
        }
    }
    return size;
}





uint64_t allocate_slot(uint64_t type) {
    for (int i = 0; i < memory_map_size; i++) {
        if (memory_map[i].type == AVAILABLE) {
            insert_mmap_entry(memory_map[i].paddr, memory_map[i].paddr, type, 0x1000);
            return memory_map[i].paddr;
        }
    }
}

uint64_t allocate_huge_slot(uint64_t type) {
    for (int i = 0; i < memory_map_size; i++) {
        if (memory_map[i].type == AVAILABLE) {
            if (memory_map[i].size > PAGE_SIZE) {
                if (memory_map[i].paddr % PAGE_SIZE) {
                    uint64_t start = (memory_map[i].paddr/PAGE_SIZE+1)*PAGE_SIZE;
                    uint64_t end = ((memory_map[i].paddr + memory_map[i].size)/PAGE_SIZE)*PAGE_SIZE;
                    if (end - start > PAGE_SIZE) {
                        insert_mmap_entry(start, start, type, PAGE_SIZE);
                        return start;
                    }
                } else {
                    insert_mmap_entry(memory_map[i].paddr, memory_map[i].paddr, type, PAGE_SIZE);
                    return memory_map[i].paddr;
                }

            }
            
        }
    }
}

uint64_t allocate_clean_slot(uint64_t type) {
    uint64_t addr = allocate_slot(type);
    memset(addr, 0x0, PAGE_SIZE_);
}

uint64_t allocate_huge_clean_slot(uint64_t type) {
    uint64_t addr = allocate_huge_slot(type);
    memset(addr, 0x0, PAGE_SIZE);
}







