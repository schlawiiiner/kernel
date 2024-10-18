#include "../../src/include/bootinfo.h"
#include "../../src/include/uint.h"
#include "../../src/include/mm/memory.h"
#include "../../src/include/mm/utils.h"
#include "../../src/include/io.h"


volatile MemoryInformation mem_info;

uint8_t priority = 0x1;
uint64_t kvaddr;
uint64_t page_counter = 0;

TotalMemoryMap memory_map[100];
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
void generate_mmap(BootInformation* multiboot_structure) {
    MemoryMapEntry * memory_map_entry = (MemoryMapEntry*)((uint64_t)bis.memory_map + 16);
    memory_map_size = (bis.memory_map->size - 16)/bis.memory_map->entry_size;
    for (int i = 0; i < memory_map_size; i++) {
        memory_map[i].paddr = memory_map_entry[i].base_addr;
        memory_map[i].vaddr = memory_map_entry[i].base_addr;
        memory_map[i].size = memory_map_entry[i].length;
        memory_map[i].type = memory_map_entry[i].type;
    }
    for (int i = 0; i < 3; i++) {
        insert_mmap_entry(kernel_mmap[i].base_addr, kernel_mmap[i].virtual_base_addr, KERNEL, kernel_mmap[i].length);
    }
    insert_mmap_entry((uint64_t)multiboot_structure, (uint64_t)multiboot_structure, BOOTINFO, ((multiboot_structure->total_size)/PAGE_SIZE_+1)*PAGE_SIZE_);
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


void allocate_page_stack() {
    mem_info.memory_size = memory_map[memory_map_size-1].paddr + memory_map[memory_map_size-1].size;
    uint64_t size = mem_info.memory_size/PAGE_SIZE_*sizeof(uint32_t);
    if (size > PAGE_SIZE) {
        // we now search for a slot that can fit a huge page
        for (int i = 0; i < memory_map_size; i++) {
            if (memory_map[i].type == AVAILABLE) {
                if (memory_map[i].size > size) {
                    uint64_t start = (memory_map[i].paddr/PAGE_SIZE+1)*PAGE_SIZE;
                    uint64_t end = ((memory_map[i].paddr + memory_map[i].size)/PAGE_SIZE)*PAGE_SIZE;
                    if (end - start > size) {
                        insert_mmap_entry(start, start, PAGESTACK, size);
                        mem_info.page_stack = (uint32_t*)start;
                        //End could also be 2 MiB alligned
                        return;
                    }
                }
            }
        }
    }
    while(1);
    //ERROR catching

}














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
    if (pdt[i] == 0x0) {
        pdt[i] = allocate_page_table() | 0x3;
    }
    pdt[i] = paddr | 0x83;
}

uint64_t mmap(uint64_t paddr, uint64_t size) {
    uint64_t offset = 0;
    uint64_t* pml4 = mem_info.PML4;
    while (offset < size) {
        if (((paddr+offset) % PAGE_SIZE) || ((size - offset) < PAGE_SIZE)) {
            map_vaddr_to_paddr(pml4, paddr+offset, paddr+offset);
            offset+=PAGE_SIZE_;
        } else {
            map_vaddr_to_huge_paddr(pml4, paddr+offset, paddr+offset);
            offset+=PAGE_SIZE;
        }
    }
    return paddr;
}


uint64_t* allocate_page_tables() {
    uint64_t p1 = allocate_huge_slot(PAGETABLE);

    memset(p1, 0x0, PAGE_SIZE);
    pool.previous = 0x0;
    pool.next = 0x0;
    pool.paddr = p1;
    pool.vaddr = p1;
    for (int i = 0; i < 8; i++) {
        pool.bitmap[8] = 0x0;
    }
    pool.bitmap[0] |= (1 << 0); // this slot gets used for the pml4
    uint64_t* pml4 = (uint64_t*)p1;
    mem_info.PML4 = pml4;


    // now we map the paddr's to the vaddr's specified in the memory map
    for (int i = 0; i < memory_map_size; i++) {
        uint64_t offset = 0;
        while(offset < memory_map[i].size) {
            if (memory_map[i].type != AVAILABLE) {
                if (((memory_map[i].paddr+offset) % PAGE_SIZE) || ((memory_map[i].size - offset) < PAGE_SIZE)) {
                    map_vaddr_to_paddr(pml4, memory_map[i].vaddr+offset, memory_map[i].paddr+offset);
                    offset+=PAGE_SIZE_;
                } else {
                    map_vaddr_to_huge_paddr(pml4, memory_map[i].vaddr+offset, memory_map[i].paddr+offset);
                    offset+=PAGE_SIZE;
                }
            } else {
                break;
            }
        }
    }
    asm volatile ("mov %0, %%cr3" :: "r"((uint64_t)pml4) : "memory");
    return pml4;
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

uint64_t allocate_page() {
    uint32_t page_id = mem_info.page_stack[mem_info.stack_ptr];
    mem_info.stack_ptr--;
    if (mem_info.stack_ptr < 0) {
        write_string_to_serial("ERROR: out of memory");
        asm volatile ("hlt");
    }
    return (uint64_t)PAGE_SIZE_*page_id;
}

void free_page(uint64_t addr) {
    uint32_t page_id = addr/PAGE_SIZE_;
    mem_info.stack_ptr++;
    mem_info.page_stack[mem_info.stack_ptr] = page_id;
}






void init_memory(BootInformation* multiboot_structure) {
    if (!((bis.present_flags & ((uint64_t)1 << 6)))){
        write_string_to_serial("ERROR: Bootloader did not provide a memory Map");
        asm volatile ("hlt");
    }
    generate_mmap(multiboot_structure);
    init_vmem();
    allocate_page_stack();
    allocate_page_tables();
    fill_page_stack();
}