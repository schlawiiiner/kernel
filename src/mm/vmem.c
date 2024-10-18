#include "../../src/include/uint.h"
#include "../../src/include/mm/memory.h"
#include "../../src/include/io.h"

VMemNodePool kernel_vmem_node_pool __attribute__((section(".sysvar")));


VMemNodePool* new_pool(VMemNodePool* previous, VMemNodePool *next) {
    write_string_to_serial("not implemented yet");
    // this is a bit tricky since this implementation should not rely on the VMem manager due to recursion
    // Alternatively we could preallocate one pool so that we do not run into this issue
    asm volatile ("hlt");
}

VMemNode* new_node() {
    VMemNodePool* pool = &kernel_vmem_node_pool;
    while(1) {
        if (pool->stack_ptr < 0) {
            // pool is exhausted
            if (!pool->next) {
                pool->next = new_pool(pool, (VMemNodePool*)0x0);
            }
            pool = pool->next;
            continue;
        } else {
            VMemNode * node = pool->stack[pool->stack_ptr];
            pool->stack_ptr--;
            return node;
        }
    }
}

void free_node(VMemNode* node) {
    VMemNodePool* pool = &kernel_vmem_node_pool;
    while (1) {
        if (((uint64_t)pool < (uint64_t)node) && ((uint64_t)node < ((uint64_t)pool+PAGE_SIZE_))) {
            pool->stack_ptr++;
            pool->stack[pool->stack_ptr] = node;
            break;
        } else {
            if (!pool->next) {
                write_string_to_serial("ERROR: trying to free a node never allocated");
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
    parent->left = 0x0;
    parent->right = 0x0;
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
            }
        } else if (parent->left->max_size > parent->right->max_size) {
            if (parent->max_size == parent->left->max_size) {
                return;
            }
            parent->max_size = parent->left->max_size;
        } else {
            if (parent->max_size == parent->right->max_size) {
                return;
            }
            parent->max_size = parent->right->max_size;
        }
        
    }

}


uint64_t kvmalloc(uint64_t size) {
    //size must be power of two
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
            if (!node->left) {
                split_node(node);
            }
            if (node->left->max_size >= size) {
                node = node->left;
            } else {
                node = node->right;
            }
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
void reserve(uint64_t addr, uint64_t size) {
    //size must be a multiple of 0x1000
    if (size % 0x1000) {
        write_string_to_serial("size should be a multiple of 0x1000");
        asm volatile ("hlt");
    }
    uint64_t max = addr + size;
    while (addr < max) {
        VMemNode* node = mem_info.KernelNode;
        while(1) {
            if (0x1000 == node->size) {
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
        addr+=0x1000;
    }
}
uint64_t kmalloc2(uint64_t size) {
    uint64_t vaddr = kvmalloc(size);
    for (uint64_t offset = 0; offset < size; offset+=PAGE_SIZE_) {
        map_vaddr_to_paddr(mem_info.PML4, vaddr+offset, allocate_page());
    }
    return vaddr;
}

void init_vmem() {
    kernel_vmem_node_pool.next = 0x0;
    kernel_vmem_node_pool.previous = 0x0;
    for (int i = 0; i < sizeof(kernel_vmem_node_pool.pool)/sizeof(VMemNode); i++) {
        kernel_vmem_node_pool.stack[i] = kernel_vmem_node_pool.pool + i;
    }
    kernel_vmem_node_pool.stack_ptr = sizeof(kernel_vmem_node_pool.pool)/sizeof(VMemNode) - 1;
    mem_info.KernelNode = kernel_vmem_node_pool.stack[kernel_vmem_node_pool.stack_ptr];
    mem_info.KernelNode->parent = 0x0;
    mem_info.KernelNode->size = 0x800000000000;
    mem_info.KernelNode->addr = 0xffff800000000000;
    mem_info.KernelNode->max_size = 0x800000000000;
    mem_info.KernelNode->left = 0x0;
    mem_info.KernelNode->right = 0x0;
    kernel_vmem_node_pool.stack_ptr--;

    // now we have to reserve the space in the kernel virtual address space that is used by the kernel binary
    for (int i = 0; i < memory_map_size; i++) {
        if ((memory_map[i].vaddr & 0xffff800000000000) == 0xffff800000000000) {
            reserve(memory_map[i].vaddr, memory_map[i].size);
        }
    }
}