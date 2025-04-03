#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"
#include "../../src/include/msi.h"

int msix_support(volatile PCI_DEV* device) {
    //check if the device supports MSIX
    if (!(device->msix_cap)) {
        return DEVICE_DOES_NOT_SUPPORT_MSIX;
    }
    
    PCI_MSIX_CAP* msix_capability = device->msix_cap;
    int bir = (int)(msix_capability->Table & 0b11);
    if (!(device->bars[bir].present)) {
        return BAR_FOR_MSIX_VECTOR_TABLE_NOT_PRESENT;
    }
    return DEVICE_SUPORTS_MSIX;
}

void msix_enable(volatile PCI_DEV* device) {
    device->msix_cap->Message_Control |= (1 << 15);
}

void msix_mask_vector(volatile PCI_DEV* device, int vector) {
    PCI_MSIX_CAP* msix_capability = device->msix_cap;
    int bir = (int)(msix_capability->Table & 0b11);
    MSIX_TableEntry * table = (MSIX_TableEntry*)(device->bars[bir].base_address + (msix_capability->Table & ~0b11));
    int table_size = (int)(0x7ff & msix_capability->Message_Control) + 1;
    if (vector < table_size) {
        table[vector].Vector_Control |= 0x1;
    } else {
        print("ERROR: Invalid MSI-X Vector\n");
        while(1);
    }
}

void msix_unmask_vector(volatile PCI_DEV* device, int vector) {
    PCI_MSIX_CAP* msix_capability = device->msix_cap;
    int bir = (int)(msix_capability->Table & 0b11);
    MSIX_TableEntry * table = (MSIX_TableEntry*)(device->bars[bir].base_address + (msix_capability->Table & ~0b11));
    int table_size = (int)(0x7ff & msix_capability->Message_Control) + 1;
    if (vector < table_size) {
        table[vector].Vector_Control &= ~(uint32_t)0x1; 
    } else {
        print("ERROR: Invalid MSI-X Vector\n");
        while(1);
    }
}

void msix_unmask_function(volatile PCI_DEV* device) {
    device->msix_cap->Message_Control  &= ~(uint16_t)(1 << 14);
}

void msix_mask_function(volatile PCI_DEV* device) {
    device->msix_cap->Message_Control |= (1 << 14);
}

void msix_configure_vector(volatile PCI_DEV* device, int vector, int apic_id, int irq) {
    PCI_MSIX_CAP* msix_capability = device->msix_cap;
    int bir = (int)(msix_capability->Table & 0b11);
    MSIX_TableEntry * table = (MSIX_TableEntry*)(device->bars[bir].base_address + (msix_capability->Table & ~0b11));
    int table_size = (int)(0x7ff & msix_capability->Message_Control) + 1;
    if (vector < table_size) {
        table[vector].Message_Address_Low = 0xfee00000 | (apic_id << 12); 
        table[vector].Message_Address_High = 0x0;
        table[vector].Message_Data = irq;
    } else {
        print("ERROR: Invalid MSI-X Vector\n");
        while(1);
    }
}


void msix_setup_table(volatile PCI_DEV* device) {
    PCI_MSIX_CAP* msix_capability = device->msix_cap;
    int bir = (int)(msix_capability->Table & 0b11);
    MSIX_TableEntry * table = (MSIX_TableEntry*)(device->bars[bir].base_address + (msix_capability->Table & ~0b11));
    int table_size = (int)(0x7ff & msix_capability->Message_Control) + 1;
    for (int i = 0; i < table_size; i++) {
        table[i].Message_Address_Low = 0x0;
        table[i].Message_Address_High = 0x0;
        table[i].Message_Data = 0x0;
        table[i].Vector_Control = 0x1;
    }
    msix_mask_function(device);
}

void __attribute__((optimize("O0"))) dump_MSI_capability(uint32_t* msi_capability) {
    
}

/*TODO: handle 64 Bit vs 32 Bit addresses
void __attribute__((optimize("O0"))) enable_MSI(int device_number) {
    volatile PCI_DEV* device = &(device_list.devices[device_number]);
    if (!(device->msi_cap_offset)) {
        print("ERROR: device ");
        printdec(device_number);
        print(" does not support MSI");
        while(1);
    }
    uint32_t* msi_capability = (uint32_t*)((uint64_t)(device->PCI_Config_Space) + device->msi_cap_offset);
    uint32_t multiple_messages = (msi_capability[0] >> 17 ) & 0b111;
    //unmask and set multiple messages enable
    msi_capability[0] = (msi_capability[0] & ~(0b10000111 << 20)) | (multiple_messages << 20);
    msi_capability[1] = (msi_capability[0] & 0xffff) | 0xfee00000;
    msi_capability[2] = 0x00000000;
    msi_capability[3] = (msi_capability[3] & 0xffffff00) | 0x23;
    //enable MSI
    msi_capability[0] |= (1<<16);

}*/

