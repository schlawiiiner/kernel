#include "../../src/include/acpi.h"
#include "../../src/include/graphics.h"
#include "../../src/include/io.h"
#include "../../src/include/pci.h"

#define ACPI_XSDT_ENTRY_OFFSET 36

volatile ACPI acpi;

void check_checksum(ACPI_Table_Header* table) {
    uint8_t* ptr = (uint8_t*)table;
    uint32_t sum = 0;
    for (int i = 0; i < table->Length; i++) {
        sum += ptr[i];
    }
    if ((sum % 0x100) != 0x00) {
        set_color(0xff0000, 0x000000);
        print("ERROR: checksum is not valid");
        while(1);
    }
}

void check_XSDT_t_checksum(XSDP_t* table) {
    uint8_t* ptr = (uint8_t*)table;
    uint8_t sum = 0;
    for (int i = 0; i < 20; i++) {
        sum += ptr[i];
    }
    if (sum != 0x00) {
        set_color(0xff0000, 0x000000);
        print("ERROR: XSDP_t checksum is not valid");
        while(1);
    }
    sum = 0;
    for (int i = 20; i < 36; i++) {
        sum += ptr[i];
    }
    if (sum != 0x00) {
        set_color(0xff0000, 0x000000);
        print("ERROR: XSDP_t checksum is not valid");
        while(1);
    }
    
}

void check_XSDT_checksum(ACPI_Table_Header* XSDT) {
    uint8_t sum = 0;
    for (int i = 0; i < XSDT->Length; i++) {
        sum += ((uint8_t*)XSDT)[i];
    }
    if (sum != 0x00) {
        set_color(0xff0000, 0x000000);
        print("ERROR: XSDT checksum is not valid");
        while(1);
    }
}

int cmp_char(char* a, char* b, int len) {
    for (int i = 0; i < len; i++) {
        if (a[i] != b[i]) {
            return 0x0;
        }
    }
    return 0x1;
}


void parse_XSDT(ACPI_Table_Header* XSDT) {
    check_XSDT_checksum(XSDT);
    uint64_t* base = (uint64_t*)((uint64_t)(XSDT) + ACPI_XSDT_ENTRY_OFFSET);

    int len = (int)((XSDT->Length - ACPI_XSDT_ENTRY_OFFSET)/8);
    for (int i = 0; i < len; i++) {
        ACPI_Table_Header* table = (ACPI_Table_Header*)(base[i]);
        check_checksum(table);
        if (cmp_char("APIC", table->Signature, 4)) {
            acpi.Flags |= 1;
            acpi.MADT = table;
        } else if (cmp_char("FACP", table->Signature, 4)) {
            acpi.Flags |= (1 << 2);
            acpi.FADT = table;
        } else if (cmp_char("MCFG", table->Signature, 4)) {
            acpi.Flags |= (1 << 1);
            acpi.MCFG = table;
        }
    }
}
