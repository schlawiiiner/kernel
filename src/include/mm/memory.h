#ifndef MEMORY_H
#define MEMORY_H
#include "../../../src/include/bootinfo.h"
#include "../../../src/include/uint.h"
#include "../../../src/include/mm/utils.h"

#define PAGE_SIZE               0x200000
#define PAGE_SIZE_              0x1000
#define PT_RANGE                (uint64_t)PAGE_SIZE_*0x200
#define PD_RANGE                (uint64_t)PT_RANGE*0x200
#define PDP_RANGE               (uint64_t)PD_RANGE*0x200
#define PML4_RANGE              (uint64_t)PDP_RANGE*0x200



#define AVAILABLE           0x1
#define RESERVED            0x2
#define ACPI                0x3
#define ACPI_NVS            0x4
#define DEFECTIVE           0x5
#define PERSISTANT          0x7
#define UNKNOWN             0x14

#define BOOTINFO            0x100
#define KERNEL              0x101
#define PAGESTACK           0x102
#define PAGETABLE           0x103

#define PRESENT             (1 << 0)
#define READ_WRITE          (1 << 1)
#define USER_SUPERVISOR     (1 << 2)
#define WRITE_THROUGH       (1 << 3)
#define CACHE_DISABLE       (1 << 4)
#define PS                  (1 << 7)
#define EXECUTE_DISABLE     (1 << 63)

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
    VMemNodePool* previous;
    VMemNodePool* next;
    int stack_ptr;
    uint8_t reserved[44];
    VMemNode* stack[37448];
    VMemNode pool[37448];
};  // This fits exactly in one huge page

typedef struct __attribute__((packed)) MemoryInformation {
    uint64_t memory_size;
    uint32_t *page_stack;
    int stack_ptr;
    uint32_t reserved;
    uint64_t* PML4;                 // be carefull when changing this struct, since the trampoline code accesses the PML4 variable in a hardcoded way
    VMemNode* KernelNode;
} MemoryInformation;



extern volatile MemoryInformation mem_info;

extern TotalMemoryMap memory_map[100];
extern int memory_map_size;
extern KernelMemoryMap kernel_mmap[3];

uint64_t mmap(uint64_t paddr, uint64_t size, uint64_t flags);
void map_to(uint64_t vaddr, uint64_t paddr, uint64_t size, uint64_t flags);
void insert_mmap_entry(uint64_t paddr, uint64_t vaddr, uint64_t type, uint64_t size);
void init_memory(BootInformation* multiboot_structure);
void map_vaddr_to_paddr(uint64_t* pml4, uint64_t vaddr, uint64_t paddr);
uint64_t allocate_physical_page();
void generate_mmap(BootInformation* multiboot_structure);
uint64_t get_available_memory_size();
void init_vmem(void);
void allocate_page_stack(void);
void fill_page_stack(void);
uint64_t allocate_huge_slot(uint64_t type);
uint64_t kmalloc2(uint64_t size);
uint64_t kvmalloc(uint64_t size);
#endif