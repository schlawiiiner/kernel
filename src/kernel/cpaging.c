#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/io.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/cpaging.h"

//paging.asm
extern uint64_t p4_table[];
extern uint64_t p3_table[];
extern uint64_t p2_table[];
extern uint64_t page_stack_bottom[];
extern uint64_t page_stack_ptr[];

//sysvar.asm
extern MemoryInformation mi;

#define PAGE_SIZE          (uint64_t)0x200000
#define NPAGES                  (int)0x2000
#define NP2_TABLES              (int)0x10
#define NP3_TABLES              (int)0x1

#define P2_ENTRIES              (uint64_t)0x200

uint64_t pop_page() {
    uint64_t addr = page_stack_bottom[(int)page_stack_ptr[0]];
    page_stack_ptr[0] = page_stack_ptr[0] - 1;
    return addr;
}
/*WARNING: This function does not check, if addr points to a valid page*/
void push_page(uint64_t addr) {
    page_stack_ptr[0] = page_stack_ptr[0] + 1;
    page_stack_bottom[(int)page_stack_ptr[0]] = addr;
}
void init_page_stack(MemoryMap* mmap) {
    page_stack_ptr[0] = 0x0;

    int n_entries = (int)((mmap->size - 16)/(mmap->entry_size));
    MemoryMapEntry* mmap_entry = (MemoryMapEntry*)((uint64_t)mmap + 16);
    mi.memory_size = mmap_entry[n_entries - 1].base_addr + mmap_entry[n_entries-1].length;

    for (int i = 0; i < n_entries; i++) {
        if (mmap_entry[i].type == 0x1) {
            //available memory
            uint64_t end_addr = mmap_entry[i].length + mmap_entry[i].base_addr;
            uint64_t addr = mmap_entry[i].base_addr;
            while (addr < end_addr) {
                if (end_addr - addr >= 0x200000) {
                    //free space large enough to fit a 2 MiB page
                    uint64_t padding = addr & 0x1fffff;
                    if (!padding) {
                        //2 MiB aligned
                        if (addr > 0x600000) {
                            p2_table[(int)(addr/0x200000)] = 0x0;
                            push_page(addr);
                        }
                        addr += 0x200000;
                    } else {
                        addr += 0x200000 - padding;
                    }
                } else {
                    addr += mmap_entry[i].length;
                }
            }
        } else {
            //the memory that is unavailable gets identity mapped to avoid unexpected behaviour
            uint64_t end_addr = mmap_entry[i].length + mmap_entry[i].base_addr;
            uint64_t addr = mmap_entry[i].base_addr - (mmap_entry[i].base_addr & 0x1fffff);
            int i = (int)(addr/0x200000);
            while (addr < end_addr) {
                p2_table[i] = addr | 0x83;
                i++;
                addr += 0x200000;
            }

        }
    }
}
void init_page_table() {
    p4_table[0] = (uint64_t)p3_table | 0x3;
    for (int i = 0; i < 0x200; i++) {
        p3_table[i] = (uint64_t)(p2_table + i*0x200) | 0x3;
    }
    /*Flush the whole TLB*/
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    asm volatile("mov %0, %%cr3" : : "r"(cr3));
}

void init_mem(MemoryMap* mmap) {
    init_page_stack(mmap);
    init_page_table();
}

int remove_page(uint64_t addr) {
    for (int i = 0; i <= page_stack_ptr[0]; i++) {
        if (addr == page_stack_bottom[i]) {
            page_stack_bottom[i] = page_stack_bottom[(int)page_stack_ptr[0]];
            page_stack_ptr[0] = page_stack_ptr[0] - 1;
            return 0;
        }
    }
    return 1;
}

uint64_t vmalloc(int size) {
    int found = 0;
    uint64_t virt_addr;
    for (int i = 0; i < 0x200*0x200; i++) {
        if (!(p2_table[i] & 0x1)) {
            //found a non-present page
            found = 1;
            for (int j = 0; j < size; j++) {
                //test if the next pages are non-present
                if ((p2_table[i+j] & 0x1)) {
                    //found a present page
                    i = i+j;
                    found = 0;
                    break;
                }
            }
            if (found) {
                virt_addr = (uint64_t)0x200000*i;
                return virt_addr;
            }
        }
    }
    printf("ERROR: vmalloc failed");
    while (1);
}

uint64_t kmalloc(int size) {
    uint64_t base_addr = vmalloc(size);
    int start_index = (int)(base_addr/0x200000);
    for (int i = start_index; i < start_index + size; i++) {
        p2_table[i] = pop_page() | 0x83;
    }
    return base_addr;
}

void kmfree(uint64_t base_addr, int size) {
    if (base_addr < mi.memory_size) {
        int base_i = (int)(base_addr/0x200000);
        uint64_t offset = 0x0;
        for (int i = base_i; i < base_i + size; i++) {
            push_page(p2_table[i] & 0x7ffffffff200000);
            p2_table[i] = 0x0;
            asm volatile ("invlpg (%0)" ::"r" (base_addr + offset) : "memory");
            offset += 0x200000;
        }
    } else {
        //MMIO
        int base_i = (int)(base_addr/0x200000);
        uint64_t offset = 0x0;
        for (int i = base_i; i < base_i + size; i++) {
            p2_table[i] = 0x0;
            asm volatile ("invlpg (%0)" ::"r" (base_addr + offset) : "memory");
            offset += 0x200000;
        }
    }
}

/*TODO: catch error and free the removed pages*/
uint64_t map_vmem_to_pmem(uint64_t base_addr, int size) {
    uint64_t virtual_base_addr = vmalloc(size);
    if (base_addr < mi.memory_size) {
        for (int i = 0; i < size; i++) {
            if (remove_page(base_addr + i*0x200000)) {
                write_string_to_serial("ERROR: physical memory is not free");
                while(1);
            }
        }
    } else {
        //MMIO, likely configuration space for hardware.
        //pass through
    }
 
    int base_i  = (int)(virtual_base_addr/0x200000);
    uint64_t virtual_offset = 0x0;
    for (int i = base_i; i < size+base_i; i++) {
        p2_table[i] = (base_addr + virtual_offset) | 0x83;
        asm volatile ("invlpg (%0)" ::"r" (virtual_base_addr + virtual_offset) : "memory");
        virtual_offset += 0x200000;
    }
    return virtual_base_addr;
}

void dump_vmem() {
    for (int i = 0; i < 0x200*0x200; i++) {
        if (p2_table[i] & 0x1) {
            printf("1");
        } else {
            printf("0");
        }
    }
    printf("\n");
}
void dump_p2() {
    for (int i = 0; i < 0x200; i++) {
        write_hex_to_serial(p2_table[i]);
        write_string_to_serial("\n");
    }
}