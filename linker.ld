ENTRY(loader)

SECTIONS
{
    . = 0x01000;
    .multiboot2 : {
        *(.multiboot2)
    }
    . = 0x02000;
    .trampoline : {
        _trampoline_start = .;
        *(.trampoline)
        . = ALIGN(4K);
        _trampoline_end = .;
    }
    . = 0x200000;
    .boot : {
        _boot_start = .;
        *(.boot)
    }
    .data :
    {
        *(.data)
    }
    .bss :
    {
        *(COMMON)
        *(.irq_table)
        *(.bss)
    }
    .info : 
    {
        kernel_mmap = .;
        QUAD(_trampoline_start);
        QUAD(_trampoline_end - _trampoline_start);
        QUAD(_trampoline_start);
        QUAD(_boot_start);
        QUAD(_boot_end - _boot_start);
        QUAD(_boot_start);
        QUAD(0x1000000);
        QUAD(_kernel_end - _kernel_start);
        QUAD(_kernel_start); 
        . = ALIGN(4K);
        _boot_end = .; 
    }
    . = 0xffff800000000000; /*gets loaded to 0x1000000*/
    .text : {
        _kernel_start = .;
        *(.text)
        *(.interrupts)
        *(.rodata)
        *(.sysvar)
        . = ALIGN(4K);
        _kernel_end = .;
    }
    
    /DISCARD/ : 
    {
        *(.comment)
        *(.eh_frame)
    }
}