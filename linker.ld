ENTRY(loader)

SECTIONS
{
    . = 0x0100000;

    .multiboot :
    {
        *(.multiboot)
    }

    .sysvar :
    {
        *(.sysvar)
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