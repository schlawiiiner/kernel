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

int msix_support(volatile PCI_DEV* device);
void msix_enable(volatile PCI_DEV* device);
void msix_mask_vector(volatile PCI_DEV* device, int vector);
void msix_unmask_vector(volatile PCI_DEV* device, int vector);
void msix_unmask_function(volatile PCI_DEV* device);
void msix_mask_function(volatile PCI_DEV* device);
void msix_configure_vector(volatile PCI_DEV* device, int vector, int apic_id, int irq);
void msix_setup_table(volatile PCI_DEV* device);
#endif