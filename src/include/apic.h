#ifndef APIC
#define APIC

#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/graphics.h"

#define APIC_BASE                       (uint64_t)0xfee00000
#define LOCAL_APIC_ID_REG_OFFSET 0x20
#define LOCAL_APIC_VERSION_REG_OFFSET 0x30
#define TPR_OFFSET 0x80
#define APR_OFFSET 0x90
#define PPR_OFFSET 0xA0
#define EOI_OFFSET 0xB0
#define RRD_OFFSET 0xC0
#define LOGICAL_DEST_REG_OFFSET 0xD0
#define DEST_FORMAT_REG_OFFSET 0xE0
#define SPURIOUS_INT_VECTOR_REG_OFFSET 0xF0
#define ISR_OFFSET 0x100
#define TMR_OFFSET 0x180
#define IRR_OFFSET 0x200
#define ERROR_STATUS_REG_OFFSET 0x280
#define CMCI_REG_OFFSET 0x2F0
#define ICR_OFFSET 0x300
#define LVT_TIMER_REG_OFFSET 0x320
#define LVT_THERMAL_SENSOR_REG_OFFSET 0x330
#define LVT_PERFORMANCE_COUNTERS_REG_OFFSET 0x340
#define LVT_LINT0_REG_OFFSET 0x350
#define LVT_LINT1_REG_OFFSET 0x360
#define LVT_ERROR_REG_OFFSET 0x370
#define INIT_COUNT_REG_OFFSET 0x380
#define CURRENT_COUNT_REG_OFFSET 0x390
#define DIVIDE_CONFIG_REG_OFFSET 0x3E0

void init_APIC(void);

void inline remap_timer(uint8_t vector) {
    uint32_t * lvt_timer_reg = (uint32_t *)(APIC_BASE + LVT_TIMER_REG_OFFSET);
    lvt_timer_reg[0] = (lvt_timer_reg[0] & 0xffffff00) | (0xff & vector);
}

void inline mask_timer(void) {
    uint32_t * lvt_timer_reg = (uint32_t *)(APIC_BASE + LVT_TIMER_REG_OFFSET);
    lvt_timer_reg[0] = lvt_timer_reg[0] | (1 << 16);
}

void inline unmask_timer(void) {
    uint32_t * lvt_timer_reg = (uint32_t *)(APIC_BASE + LVT_TIMER_REG_OFFSET);
    lvt_timer_reg[0] = lvt_timer_reg[0] & (~(1 << 16));
}

/*
0x0 : one shot
0x1 : periodic
0x2 : TSC-deadline
*/
void inline set_timer_mode(uint8_t mode) {
    uint32_t * lvt_timer_reg = (uint32_t *)(APIC_BASE + LVT_TIMER_REG_OFFSET);
    lvt_timer_reg[0] = (lvt_timer_reg[0] & ~0x60000) | ((mode & 0x3) << 17);
}

void inline send_EOI(void) {
    uint32_t * EOI_register = (uint32_t*)(APIC_BASE + EOI_OFFSET);
    EOI_register[0] = 0;
}

void inline set_initial_count(uint32_t count) {
    uint32_t *initial_count_register = (uint32_t *)(APIC_BASE + INIT_COUNT_REG_OFFSET);
    initial_count_register[0] = count;
}

/*
0x0: Divide by 2
0x1: Divide by 4
0x2: Divide by 8
0x3: Divide by 16
0x8: Divide by 32
0x9: Divide by 64
0xa: Divide by 128
0xb: Divide by 1
*/
void inline set_divide_configuration(uint8_t divide) {
    uint32_t *divide_configuration_register = (uint32_t *)(APIC_BASE + DIVIDE_CONFIG_REG_OFFSET);
    divide_configuration_register[0] = divide_configuration_register[0] | divide;
}

void set_timer();
void apic_err();
#endif