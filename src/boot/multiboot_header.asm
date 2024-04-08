%define ALIGN 1<<0
%define MEMINFO 1<<1
%define VIDINFO 1<<2
%define MAGIC 0x1badb002
%define FLAGS ALIGN | MEMINFO | VIDINFO
%define CHECKSUM -(0x1badb002 + (1<<0 | 1<<1 | 1<<2))

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    dd 0, 0, 0, 0, 0
    dd 0
    dd 1280, 720, 32
