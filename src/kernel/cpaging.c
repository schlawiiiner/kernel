#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/io.h"
#include "../../src/include/bootinfo.h"

extern uint64_t p4_table[];
extern uint64_t p3_table[];
extern uint64_t p2_table[];
extern uint64_t page_stack_bottom[];
extern uint64_t page_stack_ptr[];

#define PAGE_SIZE               (uint64_t)0x1000
#define HUGE_PAGE_SIZE          (uint64_t)0x200000
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
    page_stack_ptr[0] = (uint64_t)(page_stack_bottom - 1);
    MemoryMapEntry* mmap_entry = (MemoryMapEntry*)((uint64_t)mmap+16);
    for (int i = 0; i < (mmap->size)/(mmap->entry_size); i++) {
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
void remove_page(uint64_t addr) {
    for (int i = 0; i <= page_stack_ptr[0]; i++) {
        if (addr == page_stack_bottom[i]) {
            page_stack_bottom[i] = page_stack_bottom[(int)page_stack_ptr[0]];
            page_stack_ptr[0] = page_stack_ptr[0] - 1;
            return;
        }
    }
    fill_screen(0x000000);
    set_color(0xff0000, 0x000000);
    printf("ERROR: Page not fond");
    while(1);
}

/*TODO: check if page entry is already present*/
/*This function forces an assignment to of a physical address to a virtual address, but is slow compared to automatic assignment*/
void map_virtual_memory_to_physical_memory(uint64_t virtual_addr, uint64_t physical_addr) {
    if (virtual_addr % PAGE_SIZE != 0) {
        fill_screen(0x000000);
        set_color(0xff0000, 0x000000);
        printf("ERROR: virtual address must be page aligned");
        while(1);
    } else if (physical_addr % PAGE_SIZE != 0) {
        fill_screen(0x000000);
        set_color(0xff0000, 0x000000);
        printf("ERROR: physical address must be page aligned");
        while(1);
    }

    uint64_t p4_offset = (virtual_addr >> 39) & 0x1FF;  // Bits 39-47
    uint64_t p3_offset = (virtual_addr >> 30) & 0x1FF;  // Bits 30-38
    uint64_t p2_offset = (virtual_addr >> 21) & 0x1FF;  // Bits 21-29

    uint64_t p4_addr = (uint64_t)p3_table | 0x3;
    uint64_t p3_addr = (uint64_t)p2_table | 0x3;
    uint64_t p2_addr = (physical_addr & 0xfffffffffffff000) | 0x83;
    
    p4_table[p4_offset] = p4_addr;
    p3_table[p3_offset] = p3_addr;
    p2_table[p2_offset] = p2_addr;

    /*flush the TLB*/
    asm volatile ("invlpg (%0)" ::"r" (virtual_addr) : "memory");
    //remove_page(physical_addr);
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