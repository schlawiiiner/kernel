#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/apic.h"

void irq_handler(uint64_t irq) {
    if(irq == 0x22) {
        apic_err();
    }
    if (irq == 0x23) {
        printf("IPI");
    } else {
        printf(".");
    }   
    send_EOI();
}

void kernel_panic(char* str, int error_code, uint64_t*rsp) {
    set_color(0xff0000, 0x000000);
    fill_screen(0x0000);
    textmode* tm = (textmode*)TEXTMODE;
    tm->x_position, tm->y_position = 0,0;
    printf(str);
    char *registers[19];
    registers[0]  = "\n R15   : ";
    registers[1]  = "\n R14   : ";
    registers[2]  = "\n R13   : ";
    registers[3]  = "\n R12   : ";
    registers[4]  = "\n R11   : ";
    registers[5]  = "\n R10   : ";
    registers[6]  = "\n R9    : ";
    registers[7]  = "\n R8    : ";
    registers[8]  = "\n RBP   : ";
    registers[9]  = "\n RDI   : ";
    registers[10] = "\n RSI   : ";
    registers[11] = "\n RDX   : ";
    registers[12] = "\n RCX   : ";
    registers[13] = "\n RBX   : ";
    registers[14] = "\n RAX   : ";
    registers[15] = "\n ERR   : ";
    registers[16] = "\n RIP   : ";
    registers[17] = "\n CS    : ";
    registers[18] = "\n EFLAGS: ";
    int j = 0;
    for (int i = 0; i < 19; i++){
        if (i == 15){
            if (0 == error_code) {
                i++;
            }
        }
        printf(registers[i]);
        printhex(rsp[j]);
        j++;
    }
    while(1);
}

void division_error(uint64_t* rsp) {
    kernel_panic("\n DIVISION ERROR\n", 0, rsp);
}

void debug(uint64_t* rsp) {
    kernel_panic("\n DEBUG\n", 0, rsp);
}

void non_maskable_interrupt(uint64_t* rsp) {
    kernel_panic("\n NON MASKABLE INTERRUPT\n", 0, rsp);
}

void breakpoint(uint64_t* rsp) {
    kernel_panic("\n BREAKPOINT\n", 0, rsp);
}

void overflow(uint64_t* rsp) {
    kernel_panic("\n OVERFLOW\n", 0, rsp);
}

void bound_range_exceeded(uint64_t* rsp) {
    kernel_panic("\n BOUND RANGE EXCEEDED\n", 0, rsp);
}

void invalid_opcode(uint64_t* rsp) {
    kernel_panic("\n INVALID OPCODE\n", 0, rsp);
}

void device_not_available(uint64_t* rsp) {
    kernel_panic("\n DEVICE NOT AVAILABLE\n", 0, rsp);
}

void double_fault(uint64_t* rsp) {
    kernel_panic("\n DOUBLE FAULT\n", 1, rsp);
}

/*OUTDATED*/
void coprocessor_segment_overrun(uint64_t* rsp){
    kernel_panic("\n COPROCESSOR SEGMENT OVERRUN\n", 0, rsp);
}

void invalid_tss(uint64_t* rsp) {
    kernel_panic("\n INVALID TSS\n", 1, rsp);
}

void segment_not_present(uint64_t* rsp) {
    kernel_panic("\n SEGMENT NOT PRESENT\n", 1, rsp);
}

void stack_segment_fault(uint64_t* rsp) {
    kernel_panic("\n STACK SEGMENT FAULT\n", 1, rsp);
}
void general_protection_fault(uint64_t* rsp) {
    kernel_panic("\n GENERAL PROTECTION FAULT\n", 1, rsp);
}

void page_fault(uint64_t* rsp){
    kernel_panic("\n PAGE FAULT\n", 1, rsp);
}

/*RESERVED*/

void x87_floating_point_exception(uint64_t* rsp) {
    kernel_panic("\n X87 FLOATING POINT EXCEPTION\n", 0, rsp);
}

void alignment_check(uint64_t* rsp) {
    kernel_panic("\n ALIGNMENT CHECK\n", 1, rsp);
}

void machine_check(uint64_t* rsp) {
    kernel_panic("\n MACHINE CHECK\n", 0, rsp);
}

void simd_floating_point_exception(uint64_t* rsp) {
    kernel_panic("\n SIMD FLOATING POINT EXCEPTION\n", 0, rsp);
}

void virtualization_exception(uint64_t* rsp) {
    kernel_panic("\n VIRTUAL EXCEPTION\n", 0, rsp);
}

void control_protection_exception(uint64_t* rsp) {
    kernel_panic("\n CONTROL PROTECTION OVERRUN\n", 1, rsp);
}

/*RESERVED*/

void hypervisor_injection_exception(uint64_t* rsp) {
    kernel_panic("\n HYPERVISOR INJECTION EXCEPTION\n", 0, rsp);
}

void vmm_communication_exception(uint64_t* rsp) {
    kernel_panic("\n VMM COMMUNICATION EXCEPTION\n", 1, rsp);
}

void security_exception(uint64_t* rsp) {
    kernel_panic("\n SECURITY EXCEPTION\n", 0, rsp);
}

/*RESERVED*/

/*OUTDATED*/
void fpu_error_interrupt(uint64_t* rsp) {
    kernel_panic("\n FPU ERROR INTERRUPT\n", 0, rsp);
}
