ENTRY(loader)

SECTIONS
{
    . = 0x01000;

    .multiboot2 :
    {
        *(.multiboot2)
    }

    .sysvar :
    {
        *(.sysvar)
    }

    .trampoline :
    {
        *(.trampoline)
    }

    .text :
    {
        *(.text)
    }


    .rodata :
    {
        *(.rodata)
    }
    .data :
    {
        start_ctors = .;
        KEEP(*( .init_array ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )))
        end_ctors = .;

        *(.data)
    }
    . = 0x200000;
    .bss :
    {
        *(COMMON)
        *(.bss)
    }
    /DISCARD/ : 
    {
        *(fini_array*)
        *(.comment)
    }
}