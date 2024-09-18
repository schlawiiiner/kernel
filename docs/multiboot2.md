# Overview
The Multiboot2 specifications are an open standard describing how a boot loader can load a x86 kernel.

# Multiboot2 header
This is the Multiboot2 header present in the kernel (as now)

| Offset (bytes)    | Type | Description                                   | Value         |
| ----------------- | ---- | --------------------------------------------- | ------------- |
| 0                 | u32  | magic number                                  | 0xe85250d6    |
| 4                 | u32  | architecture  (32 bit protected mode of i386) | 0             |
| 8                 | u32  | header length (bytes)                         | 48            |
| 12                | u32  | checksum (- magic number - header length)     | 0x17adaefa    |
| 16                | u16  | type (framebuffer tag)                        | 5             |
| 18                | u16  | flags                                         | 0             |
| 20                | u32  | size                                          | 20            |
| 24                | u32  | width                                         | 1920          |
| 28                | u32  | height                                        | 1080          |
| 32                | u32  | depth                                         | 32            |
| 40                | u16  | type (end tag)                                | 0             |
| 42                | u16  | flags                                         | 0             |
| 44                | u32  | size                                          | 8             |

# Boot Information structure