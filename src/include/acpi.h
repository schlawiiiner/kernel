#ifndef ACPI_H
#define ACPI_H
#include "../../src/include/uint.h"

typedef struct __attribute__((packed)) ACPI_Table_Header {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
}ACPI_Table_Header;

typedef struct XSDP_t {
  char Signature[8];
  uint8_t Checksum;
  char OEMID[6];
  uint8_t Revision;
  uint32_t RsdtAddress;      // deprecated since version 2.0

  uint32_t Length;
  uint64_t XsdtAddress;
  uint8_t ExtendedChecksum;
  uint8_t reserved[3];
} __attribute__ ((packed)) XSDP_t;


typedef struct RSDP_t {
  char Signature[8];
  uint8_t Checksum;
  char OEMID[6];
  uint8_t Revision;
  uint32_t RsdtAddress;
} __attribute__ ((packed)) RSDP_t;

/*
------------------------------------------------
MCFG
------------------------------------------------
*/
typedef struct __attribute__((packed)) MCFG_entry {
    uint64_t base_address;
    uint16_t PCI_Segment_Group_Number;
    uint8_t start_PCI_bus_number;
    uint8_t end_PCI_bus_number;
    uint32_t reserved;
} MCFG_entry;



/*
------------------------------------------------
MADT
------------------------------------------------
*/
typedef struct __attribute__((packed)) MADT_processor_local_APIC {
  uint8_t Type;
  uint8_t Length;
  uint8_t ACPI_Processor_ID;
  uint8_t APIC_ID;
  uint32_t Flags;
} MADT_processor_local_APIC;

typedef struct __attribute__((packed)) MADT_IO_APIC {
  uint8_t Type;
  uint8_t Length;
  uint8_t IO_APIC_ID;
  uint8_t Reserved;
  uint32_t IO_APIC_Address;
  uint32_t Global_System_Interrupt_Base;
} MADT_IO_APIC;

typedef struct __attribute__((packed)) MADT_IO_Interrpt_Source_Override {
  uint8_t Type;
  uint8_t Length;
  uint8_t Bus_Source;
  uint8_t IRQ_Source;
  uint32_t Global_System_Interrupt;
  uint16_t Flags;
} MADT_IO_Interrpt_Source_Override;

typedef struct __attribute__((packed)) MADT_IO_Non_maskable_interrupt_source {
  uint8_t Type;
  uint8_t Length;
  uint8_t NMI_Source;
  uint8_t Reserved;
  uint16_t Flags;
  uint32_t Global_System_Interrupt;
} MADT_IO_Non_maskable_interrupt_source;

typedef struct __attribute__((packed)) MADT_IO_Non_maskable_interrupt {
  uint8_t Type;
  uint8_t Length;
  uint8_t ACPI_Processor_ID;
  uint16_t Flags;
  uint8_t LINT;
} MADT_IO_Non_maskable_interrupt;

typedef struct __attribute__((packed)) MADT_Local_APIC_Address_Override {
  uint8_t Type;
  uint8_t Length;
  uint16_t Reserved;
  uint64_t Local_APIC_Address;
} MADT_Local_APIC_Address_Override;

typedef struct __attribute__((packed)) MADT_Processor_Local_x2APIC {
  uint8_t Type;
  uint8_t Length;
  uint16_t Reserved;
  uint32_t Processor_local_x2APIC_ID;
  uint32_t Flags;
  uint32_t ACPI_ID;
} MADT_Processor_Local_x2APIC;

/*see sysvar.asm*/
typedef struct __attribute__((packed)) ACPI {
  uint64_t Flags;
  ACPI_Table_Header* MADT;
  ACPI_Table_Header* MCFG;
  ACPI_Table_Header* FADT;
} ACPI;

extern ACPI acpi;

void check_XSDT_t_checksum(XSDP_t* table);
void parse_XSDT(ACPI_Table_Header* XSDT);
#endif