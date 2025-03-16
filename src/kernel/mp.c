#include "../../src/include/sysvar.h"
#include "../../src/include/mp.h"
#include "../../src/include/acpi.h"
#include "../../src/include/graphics.h"
#include "../../src/include/mm/memory.h"
#include "../../src/include/apic.h"
#include "../../src/include/interrupts.h"

volatile CPUs* cpus __attribute__((section(".sysvar")));
TaskQueue task_queue __attribute__((section(".sysvar")));

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

void dequeue_task() {
    while (__atomic_exchange_n(&task_queue.lock, 1, __ATOMIC_ACQUIRE))
        while (task_queue.lock)
            __asm__ volatile("pause");

    if (task_queue.size == 0) {
        print("task queue is empty, nothing to dequeue\n");
        return;
    }
    printdec(task_queue.dequeue->ID);
    print("\n");
    task_queue.dequeue = task_queue.dequeue->next;

    __atomic_store_n(&task_queue.lock, 0, __ATOMIC_RELEASE);
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

void task_switcher(uint64_t irq) {
    dequeue_task();
}

void init_task_switcher() {
    map_isr(0x20, task_switcher);
    task_queue.task_counter = 0;
    task_queue.size = 0;
    task_queue.dequeue = (Task*)0x0;
    task_queue.enqueue = (Task*)0x0;
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
    send_IPI(1, 0x20, ICR_NO_SCHORTHAND | ICR_EDGE_TRIGGERD | ICR_FIXED);
    send_IPI(2, 0x20, ICR_NO_SCHORTHAND | ICR_EDGE_TRIGGERD | ICR_FIXED);
    send_IPI(3, 0x20, ICR_NO_SCHORTHAND | ICR_EDGE_TRIGGERD | ICR_FIXED);
    while(1);
}
