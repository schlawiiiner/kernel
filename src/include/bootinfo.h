#ifndef BOOTINFO_H
#define BOOTINFO_H
#include "../../src/include/uint.h"
#include "../../src/include/acpi.h"

typedef struct __attribute__((packed)) BootInformation {
    uint32_t total_size;
    uint32_t reserved;
} BootInformation;

typedef struct __attribute__((packed)) BasicTagStructure {
    uint32_t type;
    uint32_t size;
} BasicTagStructure;

typedef struct __attribute__((packed)) BasicMemoryInformation {
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower;
    uint32_t mem_upper;
} BasicMemoryInformation;

typedef struct __attribute__((packed, aligned(8))) ELFSectionHeader {
    uint32_t name_offset;
    uint32_t type;
    uint64_t flags;
    uint64_t address;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entsize;
} ELFSectionHeader;

/*CAUTION: The GRUB2 multiboot2 spec is wrong (as of oct 2024) this is the right tag*/
typedef struct __attribute__((packed, aligned(8))) ELFSymbols {
    uint32_t type;
    uint32_t size;
    uint32_t num;
    uint32_t entsize;
    uint16_t shndx;
    uint16_t reserved;
    ELFSectionHeader sections[];
} ELFSymbols;


typedef struct __attribute__((packed)) FramebufferInfo {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t reserved;
    uint8_t framebuffer_red_field_position;
    uint8_t framebuffer_red_mask_size;
    uint8_t framebuffer_green_field_position;
    uint8_t framebuffer_green_mask_size;
    uint8_t framebuffer_blue_field_position;
    uint8_t framebuffer_blue_mask_size;
} FramebufferInfo;

typedef struct __attribute__((packed)) MemoryMap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
} MemoryMap;

typedef struct __attribute__((packed)) MemoryMapEntry {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} MemoryMapEntry;

typedef struct __attribute__((packed)) BootInformationStructure {
    uint64_t present_flags;
    uint64_t boot_command_line;
    uint64_t boot_loader_name;
    uint64_t modules;
    BasicMemoryInformation* basic_memory_information;
    uint64_t bios_boot_device;
    MemoryMap* memory_map;
    uint64_t VBE_info;
    FramebufferInfo* framebuffer_info;
    ELFSymbols* ELF_symbols;
    uint64_t APM_table;
    uint64_t EFI_32bit_system_table_pointer;
    uint64_t EFI_64bit_system_table_pointer;
    uint64_t SMBIOS_tables;
    RSDP_t* ACPI_old_RSDP;
    XSDP_t* ACPI_new_RSDP;
    uint64_t networking_information;
    uint64_t EFI_memory_map;
    uint64_t EFI_boot_service_not_terminated;
    uint64_t EFI_32bit_image_handle_pointer;
    uint64_t EFI_64bit_image_handle_pointer;
    uint64_t image_load_base_physical_address;
} BootInformationStructure;

extern volatile BootInformationStructure bis;
#endif
