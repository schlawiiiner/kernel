bits 64
extern division_error
extern debug
extern non_maskable_interrupt
extern breakpoint
extern overflow
extern bound_range_exceeded
extern invalid_opcode
extern device_not_available
extern double_fault
extern coprocessor_segment_overrun
extern invalid_tss 
extern segment_not_present
extern stack_segment_fault
extern general_protection_fault
extern page_fault
extern x87_floating_point_exception
extern alignment_check
extern machine_check
extern simd_floating_point_exception
extern virtualization_exception
extern control_protection_exception
extern hypervisor_injection_exception
extern vmm_communication_exception
extern security_exception
extern fpu_error_interrupt

%macro PUSH_ALL_REGS 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro POP_ALL_REGS 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

section .text
int0:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call division_error
    POP_ALL_REGS
    iret
int1:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call debug
    POP_ALL_REGS
    iret
int2:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call non_maskable_interrupt
    POP_ALL_REGS
    iret
int3:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call breakpoint
    POP_ALL_REGS
    iret
int4:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call overflow
    POP_ALL_REGS
    iret
int5:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call bound_range_exceeded
    POP_ALL_REGS
    iret
int6:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call invalid_opcode
    POP_ALL_REGS
    iret
int7:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call device_not_available
    POP_ALL_REGS
    iret
int8:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call double_fault
    POP_ALL_REGS
    iret
int9:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call coprocessor_segment_overrun
    POP_ALL_REGS
    iret
int10:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call invalid_tss
    POP_ALL_REGS
    iret
int11:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call segment_not_present
    POP_ALL_REGS
    iret
int12:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call stack_segment_fault
    POP_ALL_REGS
    iret
int13:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call general_protection_fault
    POP_ALL_REGS
    iret
int14:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call page_fault
    POP_ALL_REGS
    iret
int15:
int16:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call x87_floating_point_exception
    POP_ALL_REGS
    iret
int17:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call alignment_check
    POP_ALL_REGS
    iret
int18:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call machine_check
    POP_ALL_REGS
    iret
int19:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call simd_floating_point_exception
    POP_ALL_REGS
    iret
int20:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call virtualization_exception
    POP_ALL_REGS
    iret
int21:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call control_protection_exception
    POP_ALL_REGS
    iret
int22:
int23:
int24:
int25:
int26:
int27:
int28:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call hypervisor_injection_exception
    POP_ALL_REGS
    iret
int29:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call vmm_communication_exception
    POP_ALL_REGS
    iret
int30:
    cli
    cld
    PUSH_ALL_REGS
    mov rdi, rsp
    call security_exception
    POP_ALL_REGS
    iret
int31:


exception_halt_main:
    cli
    hlt
    jmp exception_halt_main

; rax : address of handler function
; rcx : offset of IDT in bytes (e.g. for #DF 0x8*16)
make_entry:
    mov rdx, IDT
    add rdx, rcx
    mov word [rdx], ax
    mov word [rdx+2], CODE_SEG
    mov byte [rdx+4], 0x0
    mov byte [rdx+5], 0x8f
    shr rax, 16
    mov word [rdx+6], ax
    shr rax, 16
    mov dword [rdx+8], eax
    mov dword [rdx+12], 0x0
    ret

make_entry_interrupt:
    mov rdx, IDT
    add rdx, rcx
    mov word [rdx], ax
    mov word [rdx+2], CODE_SEG
    mov byte [rdx+4], 0x0
    mov byte [rdx+5], 0x8e
    shr rax, 16
    mov word [rdx+6], ax
    shr rax, 16
    mov dword [rdx+8], eax
    mov dword [rdx+12], 0x0
    ret

enable_interrupts:

disable_pic: ; otherwise the PIC-timer would trigger an double fault (it is mapped to 0x8)
    mov dx, 0x21 ;PIC1 DATA
    mov al, 0xff
    out dx, al
    mov dx, 0xa1 ; PIC2 DATA
    out dx, al
clear_IDT:
    xor rax, rax
    mov rcx, 256*2
    mov rdi, IDT
    rep stosq


make_entries:
    mov rax, int0
    mov rcx, 16*0
    call make_entry
    mov rax, int1
    mov rcx, 16*1
    call make_entry
    mov rax, int0
    mov rcx, 16*0
    call make_entry
    mov rax, int1
    mov rcx, 16*1
    call make_entry
    mov rax, int2
    mov rcx, 16*2
    call make_entry
    mov rax, int3
    mov rcx, 16*3
    call make_entry
    mov rax, int4
    mov rcx, 16*4
    call make_entry
    mov rax, int5
    mov rcx, 16*5
    call make_entry
    mov rax, int6
    mov rcx, 16*6
    call make_entry
    mov rax, int7
    mov rcx, 16*7
    call make_entry
    mov rax, int8
    mov rcx, 16*8
    call make_entry
    mov rax, int9
    mov rcx, 16*9
    call make_entry

    mov rax, int10
    mov rcx, 16*10
    call make_entry
    mov rax, int11
    mov rcx, 16*11
    call make_entry
    mov rax, int12
    mov rcx, 16*12
    call make_entry
    mov rax, int13
    mov rcx, 16*13
    call make_entry
    mov rax, int14
    mov rcx, 16*14
    call make_entry
    ;mov rax, int15
    ;mov rcx, 16*15
    ;call make_entry
    mov rax, int16
    mov rcx, 16*16
    call make_entry
    mov rax, int17
    mov rcx, 16*17
    call make_entry
    mov rax, int18
    mov rcx, 16*18
    call make_entry
    mov rax, int19
    mov rcx, 16*19
    call make_entry

    mov rax, int20
    mov rcx, 16*20
    call make_entry
    mov rax, int21
    mov rcx, 16*21
    call make_entry
    ;mov rax, int22
    ;mov rcx, 16*22
    ;call make_entry
    ;mov rax, int23
    ;mov rcx, 16*23
    ;call make_entry
    ;mov rax, int24
    ;mov rcx, 16*24
    ;call make_entry
    ;mov rax, int25
    ;mov rcx, 16*25
    ;call make_entry
    ;mov rax, int26
    ;mov rcx, 16*26
    ;call make_entry
    ;mov rax, int27
    ;mov rcx, 16*27
    ;call make_entry
    mov rax, int28
    mov rcx, 16*28
    call make_entry
    mov rax, int29
    mov rcx, 16*29
    call make_entry

    mov rax, int30
    mov rcx, 16*30
    call make_entry
    ;mov rax, int31
    ;mov rcx, 16*31
    ;call make_entry

    
load_IDT:
    cli
    lidt [IDTP]
fix_cs:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, stack_top
    mov rbp, stack_top
    sti
    jmp interrupts_enabled


