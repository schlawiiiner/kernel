#include "../../src/include/bootinfo.h"
#include "../../src/include/uint.h"
#include "../../src/include/mm/memory.h"
#include "../../src/include/io.h"


PageTablePool pool;

uint64_t allocate_page_table() {
    PageTablePool* volatile pt_pool = (PageTablePool* volatile)&pool;
    while (1) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 64; j++) {
                if (!(pt_pool->bitmap[i] & ((uint64_t)1 << j))) {
                    pt_pool->bitmap[i] |= ((uint64_t)1 << j);
                    return pt_pool->paddr + PAGE_SIZE_*(i*64 + j);
                }
            }
        }
        pt_pool = (PageTablePool* volatile)pt_pool->next;
        if (!pt_pool) {
            write_string_to_serial("pool exhausted ...");
            while (1); // Allocate new pool
        }
    }

}

void map_vaddr_to_paddr(uint64_t* pml4, uint64_t vaddr, uint64_t paddr) {
    int i = (vaddr >> 39) & 0x1ff;
    if (pml4[i] == 0x0) {
        pml4[i] = allocate_page_table() | 0x3;
    }
    uint64_t* pdpt = (uint64_t*)(pml4[i] & 0xfffffffff000);
    i = (vaddr >> 30) & 0x1ff;
    if (pdpt[i] == 0x0) {
        pdpt[i] = allocate_page_table() | 0x3;
    }
    uint64_t* pdt = (uint64_t*)(pdpt[i] & 0xfffffffff000);
    i = (vaddr >> 21) & 0x1ff;
    if (pdt[i] == 0x0) {
        pdt[i] = allocate_page_table() | 0x3;
    }
    uint64_t* pt = (uint64_t*)(pdt[i] & 0xfffffffff000);
    i = (vaddr >> 12) & 0x1ff;
    pt[i] = paddr | 0x3;
}

void map_vaddr_to_huge_paddr(uint64_t* pml4, uint64_t vaddr, uint64_t paddr) {
    int i = (vaddr >> 39) & 0x1ff;
    if (pml4[i] == 0x0) {
        pml4[i] = allocate_page_table() | 0x3;
    }
    uint64_t* pdpt = (uint64_t*)(pml4[i] & 0xfffffffff000);
    i = (vaddr >> 30) & 0x1ff;
    if (pdpt[i] == 0x0) {
        pdpt[i] = allocate_page_table() | 0x3;
    }
    uint64_t* pdt = (uint64_t*)(pdpt[i] & 0xfffffffff000);
    i = (vaddr >> 21) & 0x1ff;
    pdt[i] = paddr | 0x83;
}



void allocate_page_tables() {
    uint64_t p1 = allocate_huge_slot(PAGETABLE);

    memset(p1, 0x0, PAGE_SIZE);
    pool.previous = 0x0;
    pool.next = 0x0;
    pool.paddr = p1;
    pool.vaddr = p1;
    for (int i = 0; i < 8; i++) {
        pool.bitmap[i] = 0x0;
    }
    pool.bitmap[0] |= (1 << 0); // this slot gets used for the pml4
    uint64_t* pml4 = (uint64_t*)p1;
    mem_info.PML4 = pml4;

    // now we map the paddr's to the vaddr's specified in the memory map
    for (int i = 0; i < memory_map_size; i++) {
        uint64_t offset = 0;
        while(offset < memory_map[i].size) {
            if (memory_map[i].type != AVAILABLE) {
                if (!((memory_map[i].paddr+offset) % PAGE_SIZE) && ((memory_map[i].size-offset) >= PAGE_SIZE)) {
                    map_vaddr_to_huge_paddr(pml4, memory_map[i].vaddr+offset, memory_map[i].paddr+offset);
                    offset+=PAGE_SIZE;
                } else {
                    map_vaddr_to_paddr(pml4, memory_map[i].vaddr+offset, memory_map[i].paddr+offset);
                    offset+=PAGE_SIZE_;
                }
            } else {
                break;
            }
        }
    }
    // now we map the last PML4 table entry to itself
    pml4[511] = (uint64_t)pml4 | 0x3;

    asm volatile ("mov %0, %%cr3" :: "r"((uint64_t)pml4) : "memory");
    return;
}

void map(uint64_t vaddr, uint64_t paddr, uint64_t flags) {
    uint64_t p4_offset = (vaddr >> 39) & 0x1FF;
    uint64_t p3_offset = (vaddr >> 30) & 0x1FF;
    uint64_t p2_offset = (vaddr >> 21) & 0x1FF;
    uint64_t p1_offset = (vaddr >> 12) & 0x1FF;

    uint64_t* p4 = (uint64_t*)(0xFFFFFFFFFFFFF000); // PML4 recursive mapping
    if (!(p4[p4_offset] & 0x1)) {
        p4[p4_offset] = allocate_physical_page() | READ_WRITE | PRESENT; // Allocate new PDPT
    }
    uint64_t* p3 = (uint64_t*)(0xFFFFFFFFFFE00000 | (p4_offset << 12));
    if (!(p3[p3_offset] & 0x1)) {
        p3[p3_offset] = allocate_physical_page()  | READ_WRITE | PRESENT; // Allocate new Page Directory
    }
    uint64_t* p2 = (uint64_t*)(0xFFFFFFFFC0000000 | (p4_offset << 21) | (p3_offset << 12));
    if (!(p2[p2_offset] & 0x1)) {
        p2[p2_offset] = allocate_physical_page()  | READ_WRITE | PRESENT; // Allocate new Page Table
    }
    uint64_t* p1 = (uint64_t*)(0xFFFFFF8000000000 | (p4_offset << 30) | (p3_offset << 21) | (p2_offset << 12));
    // Map the physical address to the virtual address in the page table
    p1[p1_offset] = paddr  | flags | READ_WRITE | PRESENT; // Present and writable
    return;
}

void huge_map(uint64_t vaddr, uint64_t paddr, uint64_t flags) {
    uint64_t p4_offset = (vaddr >> 39) & 0x1FF;
    uint64_t p3_offset = (vaddr >> 30) & 0x1FF;
    uint64_t p2_offset = (vaddr >> 21) & 0x1FF;

    uint64_t* p4 = (uint64_t*)(0xFFFFFFFFFFFFF000); // PML4 recursive mapping
    if (!(p4[p4_offset] & 0x1)) {
        p4[p4_offset] = allocate_physical_page() | READ_WRITE | PRESENT; // Allocate new PDPT
    }
    uint64_t* p3 = (uint64_t*)(0xFFFFFFFFFFE00000 | (p4_offset << 12));
    if (!(p3[p3_offset] & 0x1)) {
        p3[p3_offset] = allocate_physical_page() | READ_WRITE | PRESENT; // Allocate new Page Directory
    }
    uint64_t* p2 = (uint64_t*)(0xFFFFFFFFC0000000 | (p4_offset << 21) | (p3_offset << 12));
    p2[p2_offset] = paddr | flags | PS | READ_WRITE | PRESENT;
    return;
}

/*
returns the physical address vaddr is mapped to. If vaddr is not mapped 0x0 is returned. The most significant bit indicates
that (if set) vaddr is mapped to a huge page 
*/
uint64_t get_paddr(uint64_t vaddr) {
    uint64_t p4_offset = (vaddr >> 39) & 0x1FF;
    uint64_t p3_offset = (vaddr >> 30) & 0x1FF;
    uint64_t p2_offset = (vaddr >> 21) & 0x1FF;
    uint64_t p1_offset = (vaddr >> 12) & 0x1FF;
    uint64_t offset = vaddr & 0xfff;

    uint64_t* p4 = (uint64_t*)(0xFFFFFFFFFFFFF000);
    if (!(p4[p4_offset] & PRESENT)) {
        return 0x0;
    }
    uint64_t* p3 = (uint64_t*)(0xFFFFFFFFFFE00000 | (p4_offset << 12));
    if (!(p3[p3_offset] & PRESENT)) {
        return 0x0;
    }
    uint64_t* p2 = (uint64_t*)(0xFFFFFFFFC0000000 | (p4_offset << 21) | (p3_offset << 12));
    if (!(p2[p2_offset] & PRESENT)) {
        return 0x0;
    } else if (p2[p2_offset] & PS) {
        return ((uint64_t)1 << 63) | ((p2[p2_offset] & (uint64_t)0xffffffffff000) + offset + (p1_offset << 12));
    }
    uint64_t* p1 = (uint64_t*)(0xFFFFFF8000000000 | (p4_offset << 30) | (p3_offset << 21) | (p2_offset << 12));
    return (p1[p1_offset] & (uint64_t)0xffffffffff000) + offset;
}

void unmap(uint64_t vaddr) {
    uint64_t p4_offset = (vaddr >> 39) & 0x1FF;
    uint64_t p3_offset = (vaddr >> 30) & 0x1FF;
    uint64_t p2_offset = (vaddr >> 21) & 0x1FF;
    uint64_t p1_offset = (vaddr >> 12) & 0x1FF;
    
    uint64_t* p2 = (uint64_t*)(0xFFFFFFFFC0000000 | (p4_offset << 21) | (p3_offset << 12));
    if (p2[p2_offset] & PS) {
        p2[p2_offset] = (uint64_t)0x0;
    } else {
        uint64_t* p1 = (uint64_t*)(0xFFFFFF8000000000 | (p4_offset << 30) | (p3_offset << 21) | (p2_offset << 12));
        p1[p1_offset] = (uint64_t)0x0;
    }
}
/*
maps the physical address space of size 'size' starting at base address 'paddr' to a virtual address that gets returned
note: of size is not power of two it gets magnified to a power of two
*/
uint64_t mmap(uint64_t paddr, uint64_t size, uint64_t flags) {
    // make the size a power of two, this might lead to fragmentation of virtual memory
    int i = 0;
    while (size >> i) {
        i++;
    }
    size = (uint64_t)1 << i;

    uint64_t vaddr = kvmalloc(size);
    uint64_t offset = 0;
    while (offset < size) {
        if (((paddr+offset) % PAGE_SIZE) || ((size - offset) < PAGE_SIZE)) {
            map(vaddr+offset, paddr+offset, flags);
            offset+=PAGE_SIZE_;
        } else {
            huge_map(vaddr+offset, paddr+offset, flags);
            offset+=PAGE_SIZE;
        }
    }
    return vaddr;
}

void map_to(uint64_t vaddr, uint64_t paddr, uint64_t size, uint64_t flags) {
    // make the size a power of two, this might lead to fragmentation of virtual memory
    int i = 0;
    while (size >> i) {
        i++;
    }
    size = (uint64_t)1 << i;

    uint64_t offset = 0;
    while (offset < size) {
        if (((paddr+offset) % PAGE_SIZE) || ((size - offset) < PAGE_SIZE)) {
            map(vaddr+offset, paddr+offset, flags);
            offset+=PAGE_SIZE_;
        } else {
            huge_map(vaddr+offset, paddr+offset, flags);
            offset+=PAGE_SIZE;
        }
    }
}






void init_memory(BootInformation* multiboot_structure) {
    if (!((bis.present_flags & ((uint64_t)1 << 6)))){
        write_string_to_serial("ERROR: Bootloader did not provide a memory Map");
        asm volatile ("hlt");
    }

    /*
    'fill_page_stack' must be called last, since it depends on 'allocate_page_stack' and 'allocate_page_tables'
    to reserve their space in the mmap
    */
    generate_mmap(multiboot_structure);
    init_vmem();
    allocate_page_stack();
    allocate_page_tables();
    fill_page_stack();
}