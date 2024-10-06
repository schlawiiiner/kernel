#include "../../src/include/apic.h"
#include "../../src/include/uint.h"
#include "../../src/include/interrupts.h"
#include "../../src/include/io.h"


void create_thread(uint64_t irq) {
    for (int i = 0; i < 1; i++) {
        while(__atomic_load_n((uint32_t*)(APIC_BASE+ICR_LOW_OFFSET), __ATOMIC_ACQUIRE) & ICR_SEND_PENDING);
        send_IPI(0, 0x51, ICR_ALL_INCLUDING_SELF|ICR_EDGE_TRIGGERD|ICR_ASSERT|ICR_PHYSICAL|ICR_FIXED);
        
    }
}

void start_thread(uint64_t irq) {
    print("Processor\n");
    send_EOI();
}




void init_syscalls() {
    map_isr(0x50, (func_ptr_t)create_thread);
    map_isr(0x51, (func_ptr_t)start_thread);
}
