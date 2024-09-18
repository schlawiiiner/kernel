#include "../../src/include/acpi.h"
#include "../../src/include/graphics.h"
#include "../../src/include/io.h"
#include "../../src/include/pci.h"

#define ACPI_XSDT_ENTRY_OFFSET 36

void check_RSDP_t_checksum(RSDP_t* table) {
    uint8_t* ptr = (uint8_t*)table;
    uint32_t sum;
    for (int i = 0; i < 20; i++) {
        sum += ptr[i];
    }
    if ((sum & 0x11) != 0x00) {
        set_color(0xff0000, 0x000000);
        printf("ERROR: RSDP_t checksum is not valid");
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
        printf("ERROR: XSDP_t checksum is not valid");
        while(1);
    }
    sum = 0;
    for (int i = 20; i < 36; i++) {
        sum += ptr[i];
    }
    if (sum != 0x00) {
        set_color(0xff0000, 0x000000);
        printf("ERROR: XSDP_t checksum is not valid");
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
        printf("ERROR: XSDT checksum is not valid");
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

void parse_MADT(ACPI_Table_Header* MADT) {
    uint8_t* base = (uint8_t*)MADT;
    int offset = 0x2c;
    while (offset < MADT->Length) {
        switch (base[offset]){
        case 0:
            MADT_processor_local_APIC* entry0 = (MADT_processor_local_APIC*)(base+offset);
            printhex(entry0->Type);
            printf(" ");
            printhex(entry0->ACPI_Processor_ID);
            printf(" ");
            printhex(entry0->APIC_ID);
            printf(" ");
            printhex(entry0->Flags);
            printf("\n");
            offset+= entry0->Length;
            break;
        case 1:
            MADT_IO_APIC* entry1 = (MADT_IO_APIC*)(base+offset);
            printhex(entry1->Type);
            printf(" ");
            printhex(entry1->IO_APIC_ID);
            printf(" ");
            printhex(entry1->IO_APIC_Address);
            printf(" ");
            printhex(entry1->Global_System_Interrupt_Base);
            printf("\n");
            offset+= entry1->Length;
            break;
        case 2:
            MADT_IO_Interrpt_Source_Override* entry2 = (MADT_IO_Interrpt_Source_Override*)(base+offset);
            printhex(entry2->Type);
            printf(" ");
            printhex(entry2->Bus_Source);
            printf(" ");
            printhex(entry2->IRQ_Source);
            printf(" ");
            printhex(entry2->Global_System_Interrupt);
            printf(" ");
            printhex(entry2->Flags);
            printf("\n");
            offset+= entry2->Length;
            break;
        case 3:
            MADT_IO_Non_maskable_interrupt_source* entry3 = (MADT_IO_Non_maskable_interrupt_source*)(base+offset);
            printhex(entry3->Type);
            printf(" ");
            printhex(entry3->NMI_Source);
            printf(" ");
            printhex(entry3->Flags);
            printf(" ");
            printhex(entry3->Global_System_Interrupt);
            printf("\n");
            offset+= entry3->Length;
            break;
        case 4:
            MADT_IO_Non_maskable_interrupt* entry4 = (MADT_IO_Non_maskable_interrupt*)(base+offset);
            printhex(entry4->Type);
            printf(" ");
            printhex(entry4->ACPI_Processor_ID);
            printf(" ");
            printhex(entry4->Flags);
            printf(" ");
            printhex(entry4->LINT);
            printf("\n");
            offset+= entry4->Length;
            break;
        case 5:
            MADT_Local_APIC_Address_Override* entry5 = (MADT_Local_APIC_Address_Override*)(base+offset);
            printhex(entry5->Type);
            printf(" ");
            printhex(entry5->Local_APIC_Address);
            printf("\n");
            offset+= entry5->Length;
            break;
        case 9:
            MADT_Processor_Local_x2APIC* entry9 = (MADT_Processor_Local_x2APIC*)(base+offset);
            offset+= entry9->Length;
            break;
        default:
            set_color(0xff0000, 0x000000);
            printf("\nERROR: cannot parse MADT entry of type: ");
            printhex(base[offset]);
            while(1);

        }
    }
}

void parse_FADT(ACPI_Table_Header* FADT) {

}

typedef struct __attribute__((packed)) MCFG_entry {
    uint64_t base_address;
    uint16_t PCI_Segment_Group_Number;
    uint8_t start_PCI_bus_number;
    uint8_t end_PCI_bus_number;
    uint32_t reserved;
} MCFG_entry;


void parse_MCFG(ACPI_Table_Header* MCFG) {
    for (int offset = 44; offset < MCFG->Length; offset+=16) {
        MCFG_entry* entry = (MCFG_entry*)((uint64_t)MCFG + offset);
        uint64_t addr = entry->base_address;
        for (int bus = entry->start_PCI_bus_number; bus < entry->end_PCI_bus_number; bus++) {
            for (int device = 0; device < 32; device++) {
                for (int func = 0; func < 8; func++) {
                    PCIHeader* phy_addr = (PCIHeader*)(addr + (bus << 20 | device << 15 | func << 12));
                    analyze_device(phy_addr);
                }
            }
        }
    }

}

void parse_XSDT(ACPI_Table_Header* XSDT) {
    check_XSDT_checksum(XSDT);
    uint64_t* base = (uint64_t*)((uint64_t)(XSDT) + ACPI_XSDT_ENTRY_OFFSET);

    int len = (int)((XSDT->Length - ACPI_XSDT_ENTRY_OFFSET)/8);
    for (int i = 0; i < len; i++) {
        ACPI_Table_Header* table = (ACPI_Table_Header*)(base[i]);
        if (cmp_char("APIC", table->Signature, 4)) {
            parse_MADT(table);
        } else if (cmp_char("FACP", table->Signature, 4)) {
            //parse_FADT(table);
        } else if (cmp_char("MCFG", table->Signature, 4)) {
            //parse_MCFG(table);
        }
    }
}
