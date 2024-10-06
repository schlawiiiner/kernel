#include "../../src/include/uint.h"
#define ELF_MAGIC           0x7f454c46
#define ELF_64_BIT          0x2
#define ELF_32_BIT          0x1
#define ELF_EI_VERSION      0x1
#define ELF_AMD_x86_64      0x3e
#define ELF_E_VERSION       0x1           

typedef struct __attribute__((packed)) ELF64Header {
    uint32_t ei_magic;
    uint8_t ei_class;
    uint8_t ei_data;
    uint8_t ei_version;
    uint8_t ei_OSABI;
    uint8_t ei_ABIversion;
    uint8_t ei_padding[7];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} ELF64Header;
