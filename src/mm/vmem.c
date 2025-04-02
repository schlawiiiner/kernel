#include "../../src/include/uint.h"
#include "../../src/include/mm/memory.h"
#include "../../src/include/io.h"
#include "../../src/include/graphics.h"

VMemNodePool node_pool __attribute__((section(".sysvar")));
VMemNodePool node_pool2 __attribute__((section(".sysvar")));
VMemNodePool node_pool3 __attribute__((section(".sysvar")));

VMemNodePool* preallocated_node_pool  __attribute__((section(".sysvar"))) = &node_pool3;


void init_pool(VMemNodePool* pool, VMemNodePool* previous, VMemNodePool *next) {
    pool->previous = previous;
    pool->next = next;
    pool->stack_ptr = sizeof(pool->pool)/sizeof(VMemNode)-1;
    for (long unsigned int i = 0; i < sizeof(pool->pool)/sizeof(VMemNode); i++) {
        pool->stack[i] = pool->pool + i;
    }
    if (previous) {
        previous->next = pool;
    }
    if (next) {
        next->previous = pool;
    }
    return;
}

void preallocate_node_pool(VMemNodePool* pool) {
    init_pool(preallocated_node_pool, pool, (VMemNodePool*)0x0);
    preallocated_node_pool = (VMemNodePool*)kmalloc(PAGE_SIZE_);
}

VMemNode* new_node() {
    VMemNodePool* pool = &node_pool;
    while(1) {
        if (pool->stack_ptr < 0) {
            // pool is exhausted so we preallocate a new one
            if (!pool->next) {
                write_string_to_serial("ERROR: No preallocated VMemNodePool");
                asm volatile ("hlt");
            } else {
                pool = pool->next;
                if (!(pool->next)) {
                    preallocate_node_pool(pool);
                }
                continue;
            }
        } else {
            VMemNode * node = pool->stack[pool->stack_ptr];
            pool->stack_ptr--;
            return node;
        }
    }
}

void free_node(VMemNode* node) {
    VMemNodePool* pool = &node_pool;
    while (1) {
        if (((uint64_t)pool < (uint64_t)node) && ((uint64_t)node < ((uint64_t)pool+PAGE_SIZE_))) {
            pool->stack_ptr++;
            pool->stack[pool->stack_ptr] = node;
            break;
        } else {
            if (!pool->next) {
                write_string_to_serial("ERROR: trying to free a node that was never allocated");
                asm volatile ("hlt");
            } else {
                pool = pool->next;
            }
        }
    }
}

void split_node(VMemNode* parent) {
    parent->left = new_node();
    parent->left->left = 0x0;
    parent->left->right = 0x0;
    parent->left->parent = parent;
    parent->left->size = parent->size/2;
    parent->left->max_size = parent->max_size/2;
    parent->left->addr = parent->addr;

    parent->right = new_node();
    parent->right->left = 0x0;
    parent->right->right = 0x0;
    parent->right->parent = parent;
    parent->right->size = parent->size/2;
    parent->right->max_size = parent->max_size/2;
    parent->right->addr = parent->addr + parent->size/2;

    parent->max_size /= 2;
}

void merge_nodes(VMemNode* parent) {
    parent->max_size = parent->left->max_size + parent->right->max_size;
    free_node(parent->left);
    free_node(parent->right);
    parent->left = (VMemNode*)0x0;
    parent->right = (VMemNode*)0x0;
}

void update_max_size(VMemNode* node) {

    VMemNode* parent = node;
    while (1) {
        if (!parent->parent) {
            // reached top node
            break;
        } else {
            // going one level up
            parent = parent->parent;
        }

        if (parent->left->max_size == parent->right->max_size) {
            if ((parent->left->max_size == 0) || (parent->left->max_size == parent->left->size)) {
                merge_nodes(parent);
                continue;
            } else {
                parent->max_size = parent->left->max_size;
            }
        } else if (parent->left->max_size > parent->right->max_size) {
            parent->max_size = parent->left->max_size;
        } else {
            parent->max_size = parent->right->max_size;
        }
        
    }

}


uint64_t kvmalloc(uint64_t size) {
    VMemNode* node = mem_info.KernelNode;
    if (size > node->max_size) {
        write_string_to_serial("virtual address space is not large enough");
        asm volatile ("hlt");
    }
    while (1) {
        if (node->size == size) {
            // we found a slot
            uint64_t addr = node->addr;
            node->max_size = 0x0;
            update_max_size(node);
            return addr;
        } else {
            // we have to go one level deeper
            if (node->left) {
                if ((uint64_t)node->left & ((uint64_t)1 << 63)) {
                    //
                } else {
                    print("ERROR ");
                    printhex((uint64_t)&node->left);
                    while (1);
                }
            }
            if (!node->left) {
                split_node(node);
                node = node->left;
            } else if (node->left->max_size >= size) {
                node = node->left;
            } else {
                node = node->right;
            }
        }
    }
}

void dump_tree() {
    write_string_to_serial("---------------- DUMP MEMORY TREE ----------------\n");
    VMemNode* node = mem_info.KernelNode;
    while(1) {
        if (node->max_size == node->size) {
            write_string_to_serial("free     : ");
            write_hex_to_serial(node->addr);
            write_string_to_serial("\t");
            write_hex_to_serial(node->size);
            write_string_to_serial("\n");
            while(1) {
                if (node == node->parent->right) {
                    node = node->parent;
                    if (node == mem_info.KernelNode) {
                        return;
                    }
                } else {
                    node = node->parent->right;
                    break;
                }
            }
        } else if (node->max_size == 0x0) {
            write_string_to_serial("allocated: ");
            write_hex_to_serial(node->addr);
            write_string_to_serial("\t");
            write_hex_to_serial(node->size);
            write_string_to_serial("\n");
            while(1) {
                if (node == node->parent->right) {
                    node = node->parent;
                    if (node == mem_info.KernelNode) {
                        return;
                    }
                } else {
                    node = node->parent->right;
                    break;
                }
            }
        } else {
            node = node->left;
        }
    }
}
    

void kvfree(uint64_t addr, uint64_t size) {
    VMemNode* node = mem_info.KernelNode;
    while(1) {
        if (size == node->size) {

            node->max_size = size;
            update_max_size(node);
            return;
        } else if (size > node->size) {
            // indicates size was not power of two
            write_string_to_serial("ERROR");
            asm volatile ("hlt");
        }
        if (addr >= (node->addr + node->size/2)) {
            if (!node->right) {
                split_node(node);
            }
            node = node->right;
        } else {
            if (!node->left) {
                split_node(node);
            }
            node = node->left;
        }
    }
}

uint64_t find_alignement(uint64_t addr) {
    uint64_t alignment = 12;
    uint64_t cp = addr >> alignment;
    while ((cp & 1) == 0) {
        cp >>= 1;
        alignment++;
    }
    return (uint64_t)1 << alignment;
}

void reserve(uint64_t addr, uint64_t size) {
    //size must be a multiple of 0x1000
    if (size % 0x1000) {
        write_string_to_serial("size should be a multiple of 0x1000");
        asm volatile ("hlt");
    }
    uint64_t max = addr + size;
    uint64_t remaining_size = size;
    while (addr < max) {
        size = find_alignement(addr);
        if (size > remaining_size) {
            while(size > remaining_size) {
                size = size >> 1;
            }
        }
        
        VMemNode* node = mem_info.KernelNode;
        while(1) {
            if (size == node->size) {
                node->max_size = 0x0;
                update_max_size(node);
                break;
            }
            if (addr >= (node->addr + node->size/2)) {
                if (!node->right) {
                    split_node(node);
                }
                node = node->right;
            } else {
                if (!node->left) {
                    split_node(node);
                }
                node = node->left;
            }
        }
        addr+=size;
        remaining_size-=size;
    }  
}

uint64_t kmalloc(uint64_t size) {
    uint64_t vaddr = kvmalloc(size);
    for (uint64_t offset = 0; offset < size; offset+=PAGE_SIZE_) {
        map_vaddr_to_paddr(mem_info.PML4, vaddr+offset, allocate_physical_page());
    }
    return vaddr;
}

void kfree(uint64_t addr, uint64_t size) {
    for (uint64_t offset = 0; offset < size; offset+=PAGE_SIZE_) {
        uint64_t paddr = get_paddr(addr+offset);
        if (!paddr) {
            write_string_to_serial("addr has never been allocated");
            while (1); 
        }
        if (paddr & ((uint64_t)1 << 63)) {
            //the most significant bit is set, so the address is mapped to a huge page
            paddr = paddr & ~((uint64_t)1 << 63);
            for (uint64_t off = 0; off < PAGE_SIZE; off+=PAGE_SIZE_) {
                free_physical_page(paddr+off);
            }
        } else {
            free_physical_page(paddr);
        }
    }
    kvfree(addr, size);
    unmap(addr);
}

void init_vmem() {
    /* 
    here the number of preallocated pools must be choosen carefully, 
    since it 'preallocate_new_pool' relies on 'kmalloc', which itself depends
    of the initialization of physical memory but 'init_pmem' is called after
    init 'vmem'
    */
    init_pool(&node_pool, 0x0, 0x0);
    init_pool(&node_pool2, &node_pool, 0x0);
    init_pool(&node_pool3, &node_pool2, 0x0);
    mem_info.KernelNode = node_pool.stack[node_pool.stack_ptr];
    mem_info.KernelNode->parent = 0x0;
    mem_info.KernelNode->size = 0x800000000000;
    mem_info.KernelNode->addr = 0xffff800000000000;
    mem_info.KernelNode->max_size = 0x800000000000;
    mem_info.KernelNode->left = 0x0;
    mem_info.KernelNode->right = 0x0;
    node_pool.stack_ptr--;

    // now we have to reserve the space in the kernel virtual address space that is used by the kernel binary
    for (int i = 0; i < memory_map_size; i++) {
        if ((memory_map[i].vaddr & 0xffff800000000000) == 0xffff800000000000) {
            reserve(memory_map[i].vaddr, memory_map[i].size);
        }
    }
    //now we reserve the virtual address space that is used for the recursive page table mapping
    reserve(0xFFFFFF8000000000, 8000000000);
}