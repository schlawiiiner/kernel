%define CODE_SEG     0x0008
%define DATA_SEG     0x0010

global trampoline_start
global vacant
global count
extern init_APIC
extern halt
extern mem_info

extern check_cpuid
extern check_MSR
extern check_long_mode
extern enable_paging
extern GDT.Pointer
extern IDTP

bits 16
section .trampoline
align 4096                      ; trampoline code must be 4KiB alligned
trampoline_start:
.wait:
    mov al, 1
    xchg al, [vacant]
    cmp al, 0
    jnz .wait

    xor ax, ax
    mov dx, ax
    lgdt [gdtr]

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:ProtectedMode

bits 32
ProtectedMode:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, StackTop

    call check_cpuid
    call check_MSR
    call check_long_mode
    call enable_paging
    lgdt [GDT.Pointer]
    jmp CODE_SEG:LongMode

bits 64
LongMode:
    ; now we reload the proper page table
    mov rax, [mem_info+24]
    mov cr3, rax

    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    cli
    lidt [IDTP]

    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov rsp, StackTop
    mov rbp, StackTop
    sti
    mov rax, init_APIC
    call rax
    mov cl, [count]
    add cl, 1
    mov byte [count], cl
    mov byte [vacant], 0
    mov rax, halt
    call rax

vacant:
    db 0
count:
    db 0

gdtr:
    dw gdt_end - gdt_start - 1
    dd gdt_start
gdt_start:
gdt_null:
    dq 0
gdt_code:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0
gdt_data:
    dw 0xffff
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0
gdt_end:

align 16
StackBottom:
    resb 4096
StackTop:

section .note.GNU-stack