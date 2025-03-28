#ifndef MSI_H
#define MSI_H
#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"

typedef struct __attribute__((packed)) MSIX_TableEntry {
    uint32_t Message_Address_Low;
    uint32_t Message_Address_High;
    uint32_t Message_Data;
    uint32_t Vector_Control;
} MSIX_TableEntry;

void enable_MSIX(volatile PCI_DEV* device, uint32_t irq);
void __attribute__((optimize("O0"))) enable_MSI(int device_number);
#endif