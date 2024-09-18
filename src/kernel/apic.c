#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/graphics.h"
#include "../../src/include/apic.h"
#include "../../src/include/cpaging.h"

/*see apic.asm*/
extern uint32_t enable_APIC(void);
extern uint32_t remap_APIC_registers(uint32_t addr);

void parse_MADT() {
    if (!(acpi.Flags & 0b001)) {
        printf("ERROR: No MADT table present");
        while(1);
    }
    uint8_t* base = (uint8_t*)acpi.MADT;
    int offset = 0x2c;
    while (offset < acpi.MADT->Length) {
        switch (base[offset]){
        case 0:
            MADT_processor_local_APIC* entry0 = (MADT_processor_local_APIC*)(base+offset);
            
            offset+= entry0->Length;
            break;
        case 1:
            MADT_IO_APIC* entry1 = (MADT_IO_APIC*)(base+offset);;
            offset+= entry1->Length;
            break;
        case 2:
            MADT_IO_Interrpt_Source_Override* entry2 = (MADT_IO_Interrpt_Source_Override*)(base+offset);
            
            offset+= entry2->Length;
            break;
        case 3:
            MADT_IO_Non_maskable_interrupt_source* entry3 = (MADT_IO_Non_maskable_interrupt_source*)(base+offset);
            
            offset+= entry3->Length;
            break;
        case 4:
            MADT_IO_Non_maskable_interrupt* entry4 = (MADT_IO_Non_maskable_interrupt*)(base+offset);
            
            offset+= entry4->Length;
            break;
        case 5:
            MADT_Local_APIC_Address_Override* entry5 = (MADT_Local_APIC_Address_Override*)(base+offset);
            
            offset+= entry5->Length;
            break;
        case 9:
            MADT_Processor_Local_x2APIC* entry9 = (MADT_Processor_Local_x2APIC*)(base+offset);
            offset+= entry9->Length;
            break;
        default:
            set_color(0xff0000, 0x000000);
            printf("\nERROR: cannot parse MADT entry of type: ");
            printhex(base[offset]);
            while(1);

        }
    }
}

void init_APIC(void) {
    //pic_disable();
    if (identity_map(APIC_BASE, 1)) {
        /*catch error*/
    }
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

void set_delivery_mode(int lvt_reg_offset, int delivery_mode) {
    uint32_t*  lvt_reg = (uint32_t*)(APIC_BASE + lvt_reg_offset);
    lvt_reg[0] &= ~(0b111 << 8);
    lvt_reg[0] |= (delivery_mode & 0b111) << 8;
}

void init_err() {
    uint32_t* error = (uint32_t*)(APIC_BASE + LVT_ERROR_REG_OFFSET);
    error[0] = 0x22;
}

void apic_err() {
    uint32_t* error_status_reg = (uint32_t*)(APIC_BASE + ERROR_STATUS_REG_OFFSET);
    uint32_t error_status = error_status_reg[0];
    printf("APIC ERROR:");
    while(1);
}

void set_timer() {
    init_APIC();
    uint32_t *spurious_vector = (uint32_t *)(APIC_BASE + SPURIOUS_INT_VECTOR_REG_OFFSET);
    spurious_vector[0] = 0x120;
    init_err();
    remap_timer(0x20);
    unmask_timer();
    set_timer_mode(0x1);
    set_divide_configuration(0x0);
    set_initial_count(0xf00000);
}