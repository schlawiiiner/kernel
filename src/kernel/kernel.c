#include "../../src/include/uint.h"
#include "../../src/include/bootinfo.h"
#include "../../src/include/io.h"
#include "../../src/include/graphics.h"


#define BOOT_INFO_STRUCTURE 0x100080

void parse_boot_information(BootInformation* boot_information) {
    uint64_t base = (uint64_t) boot_information;
    uint64_t offset = 0x8;
    BootInformationStructure * boot_information_structure = (BootInformationStructure*)BOOT_INFO_STRUCTURE;
    while (offset < boot_information->total_size) {
        BasicTagStructure * tag = (BasicTagStructure*)(base+offset);
        switch (tag->type) {
        case 1:
            boot_information_structure->present_flags |= (1 << 1);
            boot_information_structure->boot_command_line = base+offset;
            break;
        case 2:
            boot_information_structure->present_flags |= (1 << 2);
            boot_information_structure->boot_loader_name = base+offset;
            break;
        case 3:
            boot_information_structure->present_flags |= (1 << 3);
            boot_information_structure->modules = base+offset;
            break;
        case 4:
            boot_information_structure->present_flags |= (1 << 4);
            boot_information_structure->basic_memory_information = (BasicMemoryInformation*)(base+offset);
            break;
        case 5:
            boot_information_structure->present_flags |= (1 << 5);
            boot_information_structure->bios_boot_device = base+offset;
            break;
        case 6:
            boot_information_structure->present_flags |= (1 << 6);
            boot_information_structure->memory_map = base+offset;
            break;
        case 7:
            boot_information_structure->present_flags |= (1 << 7);
            boot_information_structure->VBE_info = base+offset;
            break;
        case 8:
            boot_information_structure->present_flags |= (1 << 8);
            boot_information_structure->framebuffer_info = (FramebufferInfo*)(base+offset);
            break;
        case 9:
            boot_information_structure->present_flags |= (1 << 9);
            boot_information_structure->ELF_symbols = base+offset;
            break;
        case 10:
            boot_information_structure->present_flags |= (1 << 10);
            boot_information_structure->APM_table = base+offset;
            break;
        case 11:
            boot_information_structure->present_flags |= (1 << 11);
            boot_information_structure->EFI_32bit_system_table_pointer = base+offset;
            break;
        case 12:
            boot_information_structure->present_flags |= (1 << 12);
            boot_information_structure->EFI_64bit_system_table_pointer = base+offset;
            break;
        case 13:
            boot_information_structure->present_flags |= (1 << 13);
            boot_information_structure->SMBIOS_tables = base+offset;
            break;
        case 14:
            boot_information_structure->present_flags |= (1 << 14);
            boot_information_structure->ACPI_old_RSDP = base+offset;
            break;
        case 15:
            boot_information_structure->present_flags |= (1 << 15);
            boot_information_structure->ACPI_new_RSDP = base+offset;
            break;
        case 16:
            boot_information_structure->present_flags |= (1 << 16);
            boot_information_structure->networking_information = base+offset;
            break;
        case 17:
            boot_information_structure->present_flags |= (1 << 17);
            boot_information_structure->EFI_memory_map = base+offset;
            break;
        case 18:
            boot_information_structure->present_flags |= (1 << 18);
            boot_information_structure->EFI_boot_service_not_terminated = base+offset;
            break;
        case 19:
            boot_information_structure->present_flags |= (1 << 19);
            boot_information_structure->EFI_32bit_image_handle_pointer = base+offset;
            break;
        case 20:
            boot_information_structure->present_flags |= (1 << 20);
            boot_information_structure->EFI_64bit_image_handle_pointer = base+offset;
            break;
        case 21:
            boot_information_structure->present_flags |= (1 << 21);
            boot_information_structure->image_load_base_physical_address = base+offset;
            break;
        }
        offset += tag->size;
        if (tag->size % 8 != 0) {
            offset += 8 - (tag->size % 8); //tags are 8 byte alligned
        }
    }

}


void kernelmain(BootInformation* multiboot_structure, unsigned int magicnumber) {
    parse_boot_information(multiboot_structure);
    BootInformationStructure * boot_information_structure = (BootInformationStructure*)BOOT_INFO_STRUCTURE;
    if ((boot_information_structure->present_flags & 0x100) >> 8) {
        init_framebuffer(boot_information_structure->framebuffer_info);
        init_text_mode(boot_information_structure->framebuffer_info);
    }
    printf("kernel was booted by: ");
    printf((char *)(boot_information_structure->boot_loader_name) + 8);
}