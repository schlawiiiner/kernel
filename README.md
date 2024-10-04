# Overview

- [x] boot
    - [x] multiboot2 grub2
    - [x] long mode
    - [x] paging
    - [x] GDT
- [ ] memory management
    - [x] page frame allocator
    - [x] memory allocator
- [ ] interrupts
    - [x] IDT
    - [x] exception handling stubs
    - [x] local APIC
    - [x] I/O APIC
- [ ] ACPI-table parsing
    - [x] XSDT
    - [x] MADT
    - [ ] FADT
    - [x] MCFG
- [ ] PCI
    - [x] device enumeration
    - [ ] MSI/MSI-X
- [ ] MP
    - [x] AP's wakeup
    - [ ] scheduler
- [ ] driver
    - [ ] xhci
    - [ ] filesystem

# Building

    make build

This command builds the kernel binary, just make sure the create the directory `bin` beforehand.

# Testing

    make qemu

With this command you can test the kernel in qemu.

# Installing

    make install

This command installs the kernel on your machine. To actually boot this kernel you need to have `grub2` installed on your machine. If you enter the `grub` bootmenu press `c` to enter the `grub` command-line. Then type:

    multiboot2 /boot/mykernel.bin
    boot

This will finally load and boot the kernel. Alternatively you can make your custom boot menu entry.
