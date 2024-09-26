%define CODE_SEG     0x0008
%define DATA_SEG     0x0010

%include "src/boot/multiboot2.asm"
global loader
global IDT
global irq_handlers
extern kernelmain
bits 32

section .text
loader:
    mov esp, stack_top
    mov ebp, stack_top
    mov dword [boot_info], ebx
    mov dword [magic], eax
    call check_multiboot
    call check_cpuid
    call check_long_mode
    call set_up_page_tables
    call enable_paging
    lgdt [GDT.Pointer]
    jmp CODE_SEG:longmode 

bits 64
longmode:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp enable_interrupts
interrupts_enabled:
    xor ecx, ecx
    xor edi, edi
    xor rsi, rsi
    mov edi, [boot_info]
    mov esi, [magic]
    call kernelmain
loop:
    hlt
    jmp loop

; Prints the error code to the serial port
bits 32
error:
    mov dx, 0x3f8
    out dx, al
    hlt

%include "src/boot/sysvar.asm"
%include "src/boot/check.asm"
%include "src/boot/paging.asm"
%include "src/boot/interrupts.asm"
%include "src/boot/apic.asm"
%include "src/boot/device.asm"


section .data
GDT:
.Null:
    dq 0x0000000000000000             ; Null Descriptor - should be present.
 
.Code:
    dq 0x00209A0000000000             ; 64-bit code descriptor (exec/read).
    dq 0x0000920000000000             ; 64-bit data descriptor (read/write).
 
align 4
    dw 0                              ; Padding to make the "address of the GDT" field aligned on a 4-byte boundary
 
.Pointer:
    dw $ - GDT - 1                    ; 16-bit Size (Limit) of GDT.
    dd GDT                            ; 32-bit Base Address of GDT. (CPU will zero extend to 64-bit)
IDTP:
    dw 256*16-1
    dq IDT

section .bss
stack_bottom:
    resb 4096
stack_top:
IDT:
    resb 4096

framebuffer:
    resb 4
magic:
    resb 4
boot_info:
    resb 4
irq_handlers:
    resb 4096