#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/io.h"
#include "../../src/include/graphics.h"
#include "../../src/include/mm/paging.h"
#include "../../src/include/apic.h"
#include "../../src/include/ioapic.h"
#include "../../src/include/pci.h"
#include "../../src/include/xhci.h"
#include "../../src/include/interrupts.h"
#include "../../src/include/thread.h"
#include "../../src/include/mm/allocator.h"

volatile BootInformationStructure bis;

void parse_boot_information(BootInformation* boot_information) {
    uint64_t base = (uint64_t) boot_information;
    uint64_t offset = 0x8;
    while (offset < boot_information->total_size) {
        BasicTagStructure * tag = (BasicTagStructure*)(base+offset);
        switch (tag->type) {
        case 1:
            bis.present_flags |= (1 << 1);
            bis.boot_command_line = base+offset;
            break;
        case 2:
            bis.present_flags |= (1 << 2);
            bis.boot_loader_name = base+offset;
            break;
        case 3:
            bis.present_flags |= (1 << 3);
            bis.modules = base+offset;
            break;
        case 4:
            bis.present_flags |= (1 << 4);
            bis.basic_memory_information = (BasicMemoryInformation*)(base+offset);
            break;
        case 5:
            bis.present_flags |= (1 << 5);
            bis.bios_boot_device = base+offset;
            break;
        case 6:
            bis.present_flags |= (1 << 6);
            bis.memory_map = (MemoryMap*)(base+offset);
            break;
        case 7:
            bis.present_flags |= (1 << 7);
            bis.VBE_info = base+offset;
            break;
        case 8:
            bis.present_flags |= (1 << 8);
            bis.framebuffer_info = (FramebufferInfo*)(base+offset);
            break;
        case 9:
            bis.present_flags |= (1 << 9);
            bis.ELF_symbols = base+offset;
            break;
        case 10:
            bis.present_flags |= (1 << 10);
            bis.APM_table = base+offset;
            break;
        case 11:
            bis.present_flags |= (1 << 11);
            bis.EFI_32bit_system_table_pointer = base+offset;
            break;
        case 12:
            bis.present_flags |= (1 << 12);
            bis.EFI_64bit_system_table_pointer = base+offset;
            break;
        case 13:
            bis.present_flags |= (1 << 13);
            bis.SMBIOS_tables = base+offset;
            break;
        case 14:
            bis.present_flags |= (1 << 14);
            bis.ACPI_old_RSDP = (RSDP_t*)(base+offset+8);
            break;
        case 15:
            bis.present_flags |= (1 << 15);
            bis.ACPI_new_RSDP = (XSDP_t*)(base+offset+8);
            break;
        case 16:
            bis.present_flags |= (1 << 16);
            bis.networking_information = base+offset;
            break;
        case 17:
            bis.present_flags |= (1 << 17);
            bis.EFI_memory_map = base+offset;
            break;
        case 18:
            bis.present_flags |= (1 << 18);
            bis.EFI_boot_service_not_terminated = base+offset;
            break;
        case 19:
            bis.present_flags |= (1 << 19);
            bis.EFI_32bit_image_handle_pointer = base+offset;
            break;
        case 20:
            bis.present_flags |= (1 << 20);
            bis.EFI_64bit_image_handle_pointer = base+offset;
            break;
        case 21:
            bis.present_flags |= (1 << 21);
            bis.image_load_base_physical_address = base+offset;
            break;
        }
        offset += tag->size;
        if (tag->size % 8 != 0) {
            offset += 8 - (tag->size % 8); //tags are 8 byte alligned
        }
    }

}

void load_drivers() {
    for (int i = 0; i < device_list.number_devices; i++) {
        if (device_list.devices[i].class == 0xc0330) {
            activate_pins();
            init_xhci_controller(i);
            int gsi = find_pins();
            deactivate_pins();
            route_hardware_interrupt(i+32, 23, (func_ptr_t)interrupt_handler);
            interrupt_handler(i+32);
            place_no_op(i);
        } else if (device_list.devices[i].class == 0x30000) {

        }
    }
}
static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    asm volatile (
        "rdtsc"            // Read Time-Stamp Counter
        : "=a"(lo), "=d"(hi) // Outputs to lo and hi
    );
    return ((uint64_t)hi << 32) | lo; // Combine hi and lo to get full 64-bit result
}


void __attribute__((optimize("O0"))) kernelmain(BootInformation* multiboot_structure, unsigned int magicnumber) {
    parse_boot_information(multiboot_structure);
    if ((bis.present_flags & (1 << 6)) >> 6) {
        init_mem(bis.memory_map);
    } else {
        write_string_to_serial("\nERROR: cannot initialize the memory, no memory map provided by the bootloader\n");
        while(1);
    }
    if ((bis.present_flags & (1 << 8)) >> 8) {
        init_graphics(bis.framebuffer_info);
    } else {
        write_string_to_serial("\nERROR: cannot initialize the graphics, no frambuffer info provided by the bootloader");
        while(1);
    }
    if ((bis.present_flags & (1 << 2)) >> 2) {
        print("\nkernel was booted by: ");
        print((char *)(bis.boot_loader_name) + 8);
        print("\n");
    } else {
        print("\nkernel was booted by: unknown bootloader\n");
    }
    if ((bis.present_flags & (1 << 15)) >> 15) {
        check_XSDT_t_checksum(bis.ACPI_new_RSDP);
        ACPI_Table_Header* xsdt = (ACPI_Table_Header*)(bis.ACPI_new_RSDP->XsdtAddress);
        parse_XSDT(xsdt);
    }
    init_default_handler();
    init_APIC();
    init_aps();

    init_syscalls();
    asm volatile ("int $0x50");
    asm volatile ("hlt");
    parse_MADT();
    init_IOAPIC();
    enumerate_devices();
    //set_timer();
    //load_drivers();
}