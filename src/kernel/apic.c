#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/graphics.h"
#include "../../src/include/apic.h"

/*see apic.asm*/
extern uint32_t enable_APIC(void);
extern uint32_t remap_APIC_registers(uint32_t addr);

void init_APIC(void) {
    //pic_disable();
    uint32_t err_code = enable_APIC(); 
    if (err_code != 0x0) {
        if (err_code == 0x1) {
            printf("no APIC");
        } else if (err_code == 0x2) {
            printf("no MSR");
        }
        while (1);
    }
    remap_APIC_registers((uint32_t)APIC_BASE);
}

void set_timer() {
    uint32_t *spurious_vector = (uint32_t *)(APIC_BASE + SPURIOUS_INT_VECTOR_REG_OFFSET);
    spurious_vector[0] = 0x120;
    init_APIC();
    remap_timer(0x20);
    unmask_timer();
    set_timer_mode(0x1);
    set_divide_configuration(0x0);
    set_initial_count(0x10000000);
}