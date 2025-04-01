#include "../../src/include/sysvar.h"
#include "../../src/include/mp.h"
#include "../../src/include/acpi.h"
#include "../../src/include/graphics.h"
#include "../../src/include/mm/memory.h"
#include "../../src/include/apic.h"
#include "../../src/include/interrupts.h"

volatile CPUs* cpus __attribute__((section(".sysvar")));
TaskQueue task_queue __attribute__((section(".sysvar")));

volatile CPU* fetch_cpu_data(uint64_t apic_id) {
    volatile CPU* cpu = (volatile CPU*)&cpus[apic_id];
    if (cpu->APIC_ID != apic_id) {
        for (int i = 0; i < cpus->number; i++) {
            if (cpus->cpu[i].APIC_ID == apic_id) {
                return &cpus->cpu[i];
            }
        }
    } else {
        return cpu;
    }
    print("invalid apic id");
    while(1);
}

void enqueue_task() {
    while (__atomic_exchange_n(&task_queue.lock, 1, __ATOMIC_ACQUIRE))
        while (task_queue.lock)
            __asm__ volatile("pause");

    Task* task = (Task*)malloc(sizeof(Task));
    task_queue.task_counter++;
    task->ID = task_queue.task_counter;
    if (task_queue.size) {
        task->next = task_queue.enqueue->previous;
        task->previous = task_queue.enqueue;
        task->next->previous = task;
        task->previous->next = task;
        task_queue.enqueue = task;
    } else {
        task->next = task;
        task->previous = task;
        task_queue.enqueue = task;
        task_queue.dequeue = task;
    }
    task_queue.size++;

    __atomic_store_n(&task_queue.lock, 0, __ATOMIC_RELEASE);
}

Task* dequeue_task() {
    while (__atomic_exchange_n(&task_queue.lock, 1, __ATOMIC_ACQUIRE))
        while (task_queue.lock)
            __asm__ volatile("pause");

    if (task_queue.size == 0) {
        print("task queue is empty, nothing to dequeue\n");
        return 0x0;
    }
    Task* task = task_queue.dequeue;
    task_queue.dequeue = task_queue.dequeue->next;

    __atomic_store_n(&task_queue.lock, 0, __ATOMIC_RELEASE);
    return task;
}

void remove_task(Task* task) {
    while (__atomic_exchange_n(&task_queue.lock, 1, __ATOMIC_ACQUIRE))
        while (task_queue.lock)
            __asm__ volatile("pause");

    if (task_queue.size == 1) {
        task_queue.enqueue = (Task*)0x0;
        task_queue.dequeue = (Task*)0x0;
    } else {
        task->previous->next = task->next;
        task->next->previous = task->previous;
    }
    task_queue.size--;
    free((uint64_t)task, sizeof(Task));

    __atomic_store_n(&task_queue.lock, 0, __ATOMIC_RELEASE);
}

void __attribute__((optimize("O1"))) task_switcher(uint64_t* rsp, uint64_t irq) {
    Task *task = dequeue_task();
    if ((Task*)0x0 == task) {
        return;
    }
    uint32_t ebx;
    asm volatile("cpuid" : "=b"(ebx) : "a"(1) : "ecx", "edx");
    uint64_t apic_id = (ebx >> 24) & 0xFF;
    
    volatile CPU* cpu = fetch_cpu_data(apic_id);
    
    if ((Task*)0x0 == cpu->Current_Task) {
        print("WARNING: Running task has no Task struct\n");
    } else {
        cpu->Current_Task->registers.RFLAGS = rsp[0];
        cpu->Current_Task->registers.R15 = rsp[1];
        cpu->Current_Task->registers.R14 = rsp[2];
        cpu->Current_Task->registers.R13 = rsp[3];
        cpu->Current_Task->registers.R12 = rsp[4];
        cpu->Current_Task->registers.R11 = rsp[5];
        cpu->Current_Task->registers.R10 = rsp[6];
        cpu->Current_Task->registers.R9 = rsp[7];
        cpu->Current_Task->registers.R8 = rsp[8];
        cpu->Current_Task->registers.RBP = rsp[9];
        cpu->Current_Task->registers.RDI = rsp[10];
        cpu->Current_Task->registers.RSI = rsp[11];
        cpu->Current_Task->registers.RDX = rsp[12];
        cpu->Current_Task->registers.RCX = rsp[13];
        cpu->Current_Task->registers.RBX = rsp[14];
        cpu->Current_Task->registers.RAX = rsp[15];
        cpu->Current_Task->registers.RIP = rsp[16];
        cpu->Current_Task->registers.CS = rsp[17];
    }

    

}

void init_task_switcher() {
    map_isr(0x20, task_switcher);
    task_queue.task_counter = 0;
    task_queue.size = 0;
    task_queue.dequeue = (Task*)0x0;
    task_queue.enqueue = (Task*)0x0;
}

void enable_cpu_features(void) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(0x07, 0x00, &eax, &ebx, &ecx, &edx);
    if (ebx & (1 << 0)) {
        /*FSGSBASE*/
        uint64_t cr4;
        __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
        cr4 |= (1 << 16);
        __asm__ volatile ("mov %0, %%cr4" :: "r"(cr4));
    } else {
        print("ERROR: FSGSBASE not supported\n");
        while(1);
    }
}

CPU* read_per_core_struct(void) {
    CPU* cpu_struct;
    asm volatile ("wrgsbase %0": "=r"(cpu_struct));
    return cpu_struct;
}

uint8_t get_apic_id(void) {
    CPU* cpu_struct = read_per_core_struct();
    return cpu_struct->APIC_ID;
}

void assign_per_core_struct(uint32_t apic_id) {
    int not_found = 1;
    for (int i = 0; i < cpus->number; i++) {
        if (apic_id == cpus->cpu[i].APIC_ID) {
            cpus->cpu[i].Initialized = 0x1;
            asm volatile ("wrgsbase %0" :: "r"(&(cpus->cpu[i])));
            not_found = 0;
            break;
        }
    }
    if (not_found) {
        print("invalid apic_id");
        while(1);
    }
}

void init_cpus() {
    if (!(acpi.Flags & 0b001)) {
        print("ERROR: No MADT table present");
        while(1);
    }
    uint8_t* base = (uint8_t*)acpi.MADT;
    int offset = 0x2c;
    int number_of_cpus = 0;
    while (offset < acpi.MADT->Length) {
        if (base[offset] == 0x0) {
            MADT_processor_local_APIC* entry0 = (MADT_processor_local_APIC*)(base+offset);
            if (entry0->Flags & 0b11) {
                // online capable
                number_of_cpus++;
            }
            offset+= entry0->Length;
        } else {
            offset += base[offset+1];
        }
    }
    cpus = (volatile CPUs*)malloc(sizeof(CPUs) + number_of_cpus*sizeof(CPU));
    cpus->number = number_of_cpus;
    int i = 0;
    offset = 0x2c;
    while (offset < acpi.MADT->Length) {
        if (base[offset] == 0x0) {
            MADT_processor_local_APIC* entry0 = (MADT_processor_local_APIC*)(base+offset);
            if (entry0->Flags & 0b11) {
                cpus->cpu[i].APIC_ID = entry0->APIC_ID;
                cpus->cpu[i].Initialized = 0x0;
                cpus->cpu[i].Processor_ID = entry0->ACPI_Processor_ID;
                cpus->cpu[i].Current_Task = (Task*)0x0;
                i++;
            }
            offset+= entry0->Length;
        } else {
            offset += base[offset+1];
        }
    }
    print("Number of CPU's: ");
    printdec(cpus->number);
    print("\n");
    init_task_switcher();
}

//no checks if apic id is valid or AP is online
void switch_cpu(int apic_id) {
    for (int i = 0; i < 1000; i++) {
        enqueue_task();
    }
    send_IPI(3, 0x20, ICR_NO_SCHORTHAND | ICR_EDGE_TRIGGERD | ICR_FIXED);
    while(1);
}
