#ifndef MEMORY_H
#define MEMORY_H
#include "../../../src/include/bootinfo.h"
#include "../../../src/include/uint.h"
#include "../../../src/include/mm/paging.h"

#define PAGE_SIZE_              0x1000
#define PT_RANGE                (uint64_t)PAGE_SIZE_*0x200
#define PD_RANGE                (uint64_t)PT_RANGE*0x200
#define PDP_RANGE               (uint64_t)PD_RANGE*0x200
#define PML4_RANGE              (uint64_t)PDP_RANGE*0x200



#define AVAILABLE           0x1
#define ACPI_INFO           0x3
#define PRESERVED           0x4
#define DEFECTIVE           0x5
#define BOOTINFO            0x6
#define KERNEL              0x7
#define PAGESTACK           0x8
#define PAGETABLE           0x9
#define UNKNOWN             0x14

typedef struct __attribute__((packed)) KernelMemoryMap {
    uint64_t base_addr;
    uint64_t length;
    uint64_t virtual_base_addr;
} KernelMemoryMap;


typedef struct __attribute__((packed)) TotalMemoryMap {
    uint64_t paddr;
    uint64_t vaddr;
    uint64_t size;
    uint64_t type;
} TotalMemoryMap;


//linked list of huge pages that get split up, into smaller for page table allocation
typedef struct PageTablePool PageTablePool;

struct __attribute__((packed)) PageTablePool {
    PageTablePool * previous;
    uint64_t paddr;
    uint64_t vaddr;
    uint64_t bitmap[8];
    PageTablePool * next;
};

typedef struct VMemNode VMemNode;
struct __attribute__((packed)) VMemNode {
    VMemNode* parent;       // Pointer to the parent node
    uint64_t size;          // Size of the address range (always power of two)
    uint64_t addr;          // Base address of the address range
    uint64_t max_size;      // Maximum size of the combined children (for quick searches)
    VMemNode* left;         // Left child (first child)
    VMemNode* right;        // Right child (second child)
};

typedef struct VMemNodePool VMemNodePool;
struct __attribute__((packed, aligned(4096))) VMemNodePool {
    VMemNodePool* previous; // Pointer to the previous pool (for linked list traversal)
    VMemNodePool* next;     // Pointer to the next pool (for linked list traversal)
    int stack_ptr;          // Stack pointer to track the available nodes
    uint32_t reserved[11];  // Padding to ensure the structure fits within one page
    VMemNode* stack[72];    // Stack of pointers to free nodes
    VMemNode pool[72];      // Actual pool of 72 preallocated VMemNode structures
};  // This fits exactly in one page

typedef struct __attribute__((packed)) MemoryInformation {
    uint64_t memory_size;
    uint32_t *page_stack;
    int stack_ptr;
    uint32_t reserved;
    uint64_t* PML4;
    VMemNode* KernelNode;
} MemoryInformation;



extern volatile MemoryInformation mem_info;

extern uint8_t priority;
extern uint64_t kvaddr;
extern uint64_t page_counter;

extern TotalMemoryMap memory_map[100];
extern int memory_map_size;
extern KernelMemoryMap kernel_mmap[3];

uint64_t mmap(uint64_t paddr, uint64_t size);
void init_memory(BootInformation* multiboot_structure);
void map_vaddr_to_paddr(uint64_t* pml4, uint64_t vaddr, uint64_t paddr);
uint64_t allocate_page();
void init_vmem(void);
#endif