#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"

extern uint64_t p4_table[];
extern uint64_t p3_table[];
extern uint64_t p2_table[];
extern uint64_t page_stack_bottom[];
extern uint64_t page_stack_ptr[];

#define PAGE_SIZE               (uint64_t)0x200000
#define NPAGES                  (int)0x2000

#define P2_ENTRIES              (uint64_t)0x200
#define P3_ENTRIES              (uint64_t)0x40000
#define P4_ENTRIES              (uint64_t)0x800000000

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
void init_page_stack() {
    uint64_t phy_mem_ptr = PAGE_SIZE*(NPAGES - 1);
    for (int i = 0; i < NPAGES; i++) {
        page_stack_bottom[i] = phy_mem_ptr;
        phy_mem_ptr =  phy_mem_ptr - PAGE_SIZE;
    }
    page_stack_ptr[0] = (uint64_t)NPAGES - 1;
    /*remove the first two pages since they were mapped in paging.asm*/
    pop_page();
    pop_page();
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
    printf("Error: Page not fond");
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
    remove_page(physical_addr);
}