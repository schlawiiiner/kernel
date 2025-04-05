global enable_APIC
global remap_APIC_registers
global irq_probe
global ioapic_list

section .text
enable_APIC:
    mov eax, 0x1
    cpuid
    bt edx, 9
    jnc .noLAPIC
    bt edx, 5
    jnc .noMSR

    mov ecx, 0x1b       ; address 0x1b for IA32_APIC_BASE MSR
    rdmsr
    or eax, (1 << 11)   ; set bit 11 for enable
    wrmsr
    xor rax, rax
    ret

.noLAPIC:
    mov rax, 1
    ret

.noMSR:
    mov rax, 2
    ret

; only 32 bit address with lower 12 cleard bits supported
remap_APIC_registers:
    push rcx
    push rax
    mov rax, 0xfffff000
    and rdi, rax        ; clear the lower 12 bits 
    mov rcx, 0x1b       ; address 0x1b for IA32_APIC_BASE MSR
    rdmsr
    
    and rax, 0x00000fff ; mask the upper bits
    or rax, rdi
    wrmsr
    pop rax
    pop rcx
    ret

section .data
irq_probe:
    dq 1

section .bss
ioapic_list:
    resd 8      ; number of I/O APICS
    resd 8      ; number global system interrupts
    resb 96     ; 96 global pins
    resb 4*2    ; 4 IOAPICS supported (see struct IOAPIC ioapic.c)

section .note.GNU-stack