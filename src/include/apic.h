#ifndef APIC_H
#define APIC_H

#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/graphics.h"

/*
----------------------------------------------------------
Registers
----------------------------------------------------------
*/
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
#define ICR_LOW_OFFSET 0x300
#define ICR_HIGH_OFFSET 0x310
#define LVT_TIMER_REG_OFFSET 0x320
#define LVT_THERMAL_SENSOR_REG_OFFSET 0x330
#define LVT_PERFORMANCE_COUNTERS_REG_OFFSET 0x340
#define LVT_LINT0_REG_OFFSET 0x350
#define LVT_LINT1_REG_OFFSET 0x360
#define LVT_ERROR_REG_OFFSET 0x370
#define INIT_COUNT_REG_OFFSET 0x380
#define CURRENT_COUNT_REG_OFFSET 0x390
#define DIVIDE_CONFIG_REG_OFFSET 0x3E0

/*
----------------------------------------------------
Local Vector Table
----------------------------------------------------
*/

#define LVT_MASK                        1 << 16

#define LVT_ONE_SHOT                    0b00 << 17
#define LVT_PERIODIC                    0b01 << 17
#define LVT_TSC_DEADLINE                0b10 << 17

#define LVT_EDGE_TRIGGERED              0b0 << 15
#define LVT_LEVEL_TRIGGERED             0b1 << 15

#define LVT_FIXED                       0b000 << 8
#define LVT_SMI                         0b010 << 8
#define LVT_NMI                         0b100 << 8
#define LVT_EXT_INT                     0b111 << 8
#define LVT_INIT                        0b101 << 8

#define LVT_REMOTE_IRR                  0b1 << 14

#define LVT_INTERRUPT_INPUT_PIN_POLARITY    0b1 << 13

/*
-----------------------------------------------------
ICR Register
-----------------------------------------------------
*/
#define ICR_FIXED                   0b000 << 8
#define ICR_LOWEST_PRIORITY         0b001 << 8
#define ICR_SMI                     0b010 << 8
#define ICR_NMI                     0b100 << 8
#define ICR_INIT                    0b101 << 8
#define ICR_STARTUP                 0b110 << 8

#define ICR_LOGICAL                 0b1 << 11
#define ICR_PHYSICAL                0b0 << 11

#define ICR_DEASSERT                0b0 << 14
#define ICR_ASSERT                  0b1 << 14

#define ICR_EDGE_TRIGGERD           0b0 << 15
#define ICR_LEVEL_TRIGGERD          0b1 << 15

#define ICR_NO_SCHORTHAND               0b00 << 18
#define ICR_SELF                        0b01 << 18
#define ICR_ALL_INCLUDING_SELF          0b10 << 18
#define ICR_ALL_EXCLUDING_SELF          0b11 << 18

#define ICR_SEND_PENDING                0b1 << 12

void init_APIC(void);

inline static void remap_timer(uint8_t vector) {
    uint32_t * lvt_timer_reg = (uint32_t *)(APIC_BASE + LVT_TIMER_REG_OFFSET);
    lvt_timer_reg[0] = (lvt_timer_reg[0] & 0xffffff00) | (0xff & vector);
}

inline static void mask_timer(void) {
    uint32_t * lvt_timer_reg = (uint32_t *)(APIC_BASE + LVT_TIMER_REG_OFFSET);
    lvt_timer_reg[0] = lvt_timer_reg[0] | (1 << 16);
}

inline static void unmask_timer(void) {
    uint32_t * lvt_timer_reg = (uint32_t *)(APIC_BASE + LVT_TIMER_REG_OFFSET);
    lvt_timer_reg[0] = lvt_timer_reg[0] & (~(1 << 16));
}

/*
0x0 : one shot
0x1 : periodic
0x2 : TSC-deadline
*/
inline static void set_timer_mode(uint8_t mode) {
    uint32_t * lvt_timer_reg = (uint32_t *)(APIC_BASE + LVT_TIMER_REG_OFFSET);
    lvt_timer_reg[0] = (lvt_timer_reg[0] & ~0x60000) | ((mode & 0x3) << 17);
}

static inline void send_EOI(void) {
    uint32_t * EOI_register = (uint32_t*)(APIC_BASE + EOI_OFFSET);
    EOI_register[0] = 0;
}

inline static void set_initial_count(uint32_t count) {
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
inline static void set_divide_configuration(uint8_t divide) {
    uint32_t *divide_configuration_register = (uint32_t *)(APIC_BASE + DIVIDE_CONFIG_REG_OFFSET);
    divide_configuration_register[0] = divide_configuration_register[0] | divide;
}

void set_timer();
void apic_err(uint64_t* rsp, uint64_t irq);
void parse_MADT();
void send_IPI(int APIC_ID, int vector, int flags);
void init_aps();
void check_lapic_irr(uint8_t vector);
void check_lapic_isr(uint8_t vector);
#endif