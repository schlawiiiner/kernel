%if MAX_RAMSIZE
%else
    %define MAX_RAMSIZE     0x400000000
%endif
%define PAGESIZE            0x200000

bits 32

global p4_table
global p3_table
global p2_table

section .boot
set_up_page_tables:
    ; map first P4 entry to P3 table
    mov eax, p3_table
    or eax, 0b11 ; present + writable
    mov [p4_table], eax
    
    mov eax, p3_table_high
    or eax, 0b11
    mov [p4_table+256*8], eax

    mov eax, p2_table
    or eax, 0b11 ; present + writable
    mov [p3_table], eax
    
    mov eax, p2_table_high
    or eax, 0b11 ; present + writable
    mov [p3_table_high], eax
    
    ; map each P2 entry to a huge 2MiB page
    mov ecx, 0         ; counter variable

.map_p2_table_low:
    ; map ecx-th P2 entry to a huge page that starts at address 2MiB*ecx
    mov eax, 0x200000  ; 2MiB
    mul ecx            ; start address of ecx-th page
    or eax, 0b10000011 ; present + writable + huge
    mov [p2_table + ecx * 8], eax ; map ecx-th entry

    inc ecx            ; increase counter
    cmp ecx, 512       ; if counter == 512, the whole P2 table is mapped
    jl .map_p2_table_low  ; else map the next entry

    xor ecx, ecx
    
.map_p2_table_high:
    ; map ecx-th P2 entry to a huge page that starts at address 2MiB*ecx
    mov eax, 0x0200000                  ; load addr of high half kernel
    mul ecx
    add eax, 0x1000000
    or eax, 0b10000011                  ; present + writable + huge
    mov [p2_table_high + ecx * 8], eax            ; map the load address of the high half kernel to the high half

    inc ecx
    cmp ecx, 512
    jl .map_p2_table_high
    ret

enable_paging:
    ; load P4 to cr3 register (cpu uses this to access the P4 table)
    mov eax, p4_table
    mov cr3, eax

    ; enable PAE-flag in cr4 (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; set the long mode bit in the EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging in the cr0 register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    ret


section .bss
; this is just a temporary page table
align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p3_table_high:
    resb 4096
p2_table:
    resb 4096*512
p2_table_high:
    resb 4092*512