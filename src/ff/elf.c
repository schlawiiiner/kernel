#include "../../src/include/elf.h"

int check_elf_header(ELF64Header* header) {
    if (header->ei_magic != ELF_MAGIC) {
        return -1;
    }
    if (header->ei_class != ELF_64_BIT) {
        return -2;
    }
    if (header->ei_version != ELF_EI_VERSION) {
        return -4;
    }
    if (header->e_machine != ELF_AMD_x86_64) {
        return -8;
    }
    if (header->e_version != ELF_E_VERSION) {
        return -16;
    }
    return 0;
}