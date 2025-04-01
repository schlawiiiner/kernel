#ifndef MSI_H
#define MSI_H
#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"

#define DEVICE_DOES_NOT_SUPPORT_MSIX            0x1
#define BAR_FOR_MSIX_VECTOR_TABLE_NOT_PRESENT   0x2
#define DEVICE_SUPORTS_MSIX                     0x0

typedef struct __attribute__((packed)) MSIX_TableEntry {
    uint32_t Message_Address_Low;
    uint32_t Message_Address_High;
    uint32_t Message_Data;
    uint32_t Vector_Control;
} MSIX_TableEntry;

void enable_MSIX(volatile PCI_DEV* device, uint32_t irq);
#endif