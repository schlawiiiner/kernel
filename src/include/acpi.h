#ifndef ACPI
#define ACPI
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

void check_RSDP_t_checksum(RSDP_t* table);
void check_XSDT_t_checksum(XSDP_t* table);
void parse_XSDT(ACPI_Table_Header* XSDT);
#endif