#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/graphics.h"
#include "../../src/include/apic.h"
#include "../../src/include/mm/paging.h"
#include "../../src/include/ioapic.h"
#include "../../src/include/interrupts.h"
#include "../../src/include/io.h"

/*see apic.asm*/
extern uint32_t enable_APIC(void);
extern uint32_t remap_APIC_registers(uint32_t addr);

/*see mp.asm*/
extern void trampoline_start(void);
extern volatile uint8_t vacant[];
extern volatile uint8_t count[];

void parse_MADT() {
    if (!(acpi.Flags & 0b001)) {
        print("ERROR: No MADT table present");
        while(1);
    }
    uint8_t* base = (uint8_t*)acpi.MADT;
    int offset = 0x2c;
    while (offset < acpi.MADT->Length) {
        switch (base[offset]){
        case 0:
            MADT_processor_local_APIC* entry0 = (MADT_processor_local_APIC*)(base+offset);
            print("Processor ");
            printdec(entry0->APIC_ID);
            if (entry0->Flags & 0b11) {
                print(": online capable\n");
            } else {
                print(": not online capable\n");
            }
            offset+= entry0->Length;
            break;
        case 1:
            MADT_IO_APIC* entry1 = (MADT_IO_APIC*)(base+offset);
            map_IOAPIC_MMIO(entry1);            
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
            print("\nERROR: cannot parse MADT entry of type: ");
            printhex(base[offset]);
            while(1);

        }
    }
}



void set_delivery_mode(int lvt_reg_offset, int delivery_mode) {
    uint32_t*  lvt_reg = (uint32_t*)(APIC_BASE + lvt_reg_offset);
    lvt_reg[0] &= ~(0b111 << 8);
    lvt_reg[0] |= (delivery_mode & 0b111) << 8;
}

void init_err() {
    uint32_t* error_reg = (uint32_t*)(APIC_BASE + LVT_ERROR_REG_OFFSET);
    uint32_t error = error_reg[0];
    error &= ~0b1000000011111111;
    error |= 0x22;
    error_reg[0] = error;
}

void  __attribute__((optimize("O0"))) send_IPI(int APIC_ID, int vector, int flags) {

    
    uint32_t* icr = (uint32_t*)(APIC_BASE + ICR_HIGH_OFFSET);

    uint32_t icr_read = icr[0];
    icr_read &= 0xffffff;
    icr_read |= APIC_ID << 0x18;
    icr[0] = icr_read;

    icr = (uint32_t*)(APIC_BASE + ICR_LOW_OFFSET);

    int timeout = 0x100000;
    while (icr[0] & (1 << 12)) {
        if (timeout == 0) {
            print("ERROR: timeout while waiting to send IPI");
            while(1);
        }
        timeout--;
    }

    icr_read = icr[0];
    icr_read &= 0b11111111111100110011000000000000;
    icr_read |= flags | (vector & 0xff);
    icr[0] = icr_read;   
}

void __attribute__((optimize("O0"))) apic_err() {
    set_color(0xff0000, 0x000000);
    fill_screen(0x0000);
    set_cursor(0, 0);

    print("\nAPIC ERROR\n\n");

    //Intel's manual says that error_status_reg bust first be written to befor reading it
    uint32_t* error_status_reg = (uint32_t*)(APIC_BASE + ERROR_STATUS_REG_OFFSET);
    error_status_reg[0] = 0;
    uint32_t error_status = error_status_reg[0];

    
    if (error_status & 1) {
        print("Send Checksum Error\n");
    }
    if (error_status & 2) {
        print("Receive Checksum Error\n");
    }
    if (error_status & 4) {
        print("Send Accept Error\n");
    }
    if (error_status & 8) {
        print("Receive Accept Error\n");
    }
    if (error_status & 16) {
        print("Redirectable IPI\n");
    }
    if (error_status & 32) {
        print("Send Illegal Vector\n");
    }
    if (error_status & 64) {
        print("Receive Illegal Vector\n");
    }
    if (error_status & 128) {
        print("Illegal Register Address\n");
    }
    while(1);
}

void __attribute__((optimize("O0"))) dump_apic_regs(void) {
    uint64_t base = (uint64_t)(APIC_BASE);
    for (int i = 0; i < 0x400; i+=0x10) {
        printhex(APIC_BASE + i);
        print(" : ");
        printhex(((uint32_t*)(base+i))[0]);
        print("\n");
    }
}

void set_timer() {
    remap_timer(0x20);
    unmask_timer();
    set_timer_mode(0x1);
    set_divide_configuration(0x0);
    set_initial_count(0xf00000);
}

void __attribute__((optimize("O0"))) init_aps(void) {
    vacant[0] = 0;
    count[0] = 1;

    //INIT IPI
    send_IPI(0,0, ICR_ALL_EXCLUDING_SELF|ICR_LEVEL_TRIGGERD|ICR_ASSERT|ICR_PHYSICAL|ICR_INIT);
    int timeout = 0x100000;
    while(timeout != 0) {
        timeout--;
    }
    //SIPI
    send_IPI(0, (uint8_t)((uint64_t)trampoline_start/0x1000), ICR_ALL_EXCLUDING_SELF|ICR_EDGE_TRIGGERD|ICR_ASSERT|ICR_PHYSICAL|ICR_STARTUP);
    timeout = 0x200000;
    while(timeout != 0) {
        timeout--;
    }
    //SIPI
    send_IPI(0, (uint8_t)((uint64_t)trampoline_start/0x1000), ICR_ALL_EXCLUDING_SELF|ICR_EDGE_TRIGGERD|ICR_ASSERT|ICR_PHYSICAL|ICR_STARTUP);
    timeout = 0x200000;
    while(timeout != 0) {
        timeout--;
    }
    while(__atomic_load_n(vacant, __ATOMIC_ACQUIRE));
    print("all APs ready\n\n");
    return;
}

void init_APIC(void) {
    if (identity_map(APIC_BASE, 1, 1, 0, 0, 0)) {
        /*catch error*/
    }
    
    uint32_t err_code = enable_APIC(); 
    if (err_code != 0x0) {
        if (err_code == 0x1) {
            print("no APIC");
        } else if (err_code == 0x2) {
            print("no MSR");
        }
        while (1);
    }
    remap_APIC_registers((uint32_t)APIC_BASE);
    print("initializing processor: ");
    uint32_t* id = (uint32_t*)(APIC_BASE+LOCAL_APIC_ID_REG_OFFSET);
    printdec((uint64_t)(id[0] >> 24));
    print("\n");
    uint32_t *spurious_vector = (uint32_t *)(APIC_BASE + SPURIOUS_INT_VECTOR_REG_OFFSET);
    spurious_vector[0] = 0x120;
    init_err();
}