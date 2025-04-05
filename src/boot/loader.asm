%define KERNEL_CODE_SEG     0x0008
%define KERNEL_DATA_SEG     0x0010

%define USER_CODE_SEG       0x0018
%define USER_DATA_SEG       0x0020

global loader
global GDT
global TSS
global error
global stack_top
global interrupts_enabled
global GDT.Pointer

extern kernelmain
extern printhex
extern check_multiboot
extern check_cpuid
extern check_MSR
extern check_long_mode
extern set_up_page_tables
extern enable_paging
extern enable_interrupts
bits 32

section .boot
loader:
    mov esp, stack_top
    mov ebp, stack_top
    mov dword [boot_info], ebx
    call check_multiboot
    call check_cpuid
    call check_MSR
    call check_long_mode
    call set_up_page_tables
    call enable_paging
    lgdt [GDT.Pointer]
    jmp KERNEL_CODE_SEG:longmode 

bits 64
longmode:
    mov ax, KERNEL_DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp enable_interrupts
interrupts_enabled:
    xor rdi, rdi
    mov edi, [boot_info]
    mov rax, kernelmain
    call rax

section .boot
; Prints the error code to the serial port
bits 32
error:
    mov dx, 0x3f8
    out dx, al
    hlt

section .data
GDT:
.Null:
    dq 0x0000000000000000             ; Null Descriptor - should be present.
.KernelCode:
    dq 0x00209A0000000000             ; 64-bit ring 0 code descriptor (exec/read).
.KernelData:
    dq 0x0000920000000000             ; 64-bit ring 0 data descriptor (read/write).
.UserCode:
    dq 0x0020FA0000000000             ; 64-bit ring 3 code descriptor (exec/read)
.UserData:
    dq 0x0000F20000000000             ; 64-bit ring 3 data descriptor (read/write).
.TSS:
    dq 0x0000000000000000
    dq 0x0000000000000000

    dq 0x0000000000000000
    dq 0x0000000000000000
align 4
    dw 0                              ; Padding to make the "address of the GDT" field aligned on a 4-byte boundary
 
.Pointer:
    dw $ - GDT - 1                    ; 16-bit Size (Limit) of GDT.
    dq GDT                            ; 64-bit Base Address of GDT
TSS:
    dd 0
    dq ring0_stack_bottom
    dq 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    dd 0



section .bss
align 16
stack_bottom:
    resb 4096*8
stack_top:
align 16
ring0_stack_bottom:
    resb 4096
ring0_stack_top:
framebuffer:
    resb 4
boot_info:
    resb 4



section .note.GNU-stack
; This gets rid of the following warning message
; ld: warning: bin/loader.o: missing .note.GNU-stack section implies executable stack
; ld: NOTE: This behaviour is deprecated and will be removed in a future version of the linker