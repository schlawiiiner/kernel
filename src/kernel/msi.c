#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"
#include "../../src/include/msi.h"

void __attribute__((optimize("O0"))) enable_MSIX(volatile PCI_DEV* device, uint32_t irq) {
    //check if the device supports MSIX
    if (!(device->msix_cap_offset)) {
        print("ERROR: device does not support MSIX");
        while(1);
    }
    
    PCI_MSIX_CAP* msix_capability = (PCI_MSIX_CAP*)((uint64_t)(device->PCI_Config_Space) + device->msix_cap_offset);
    int bir = (int)(msix_capability->Table & 0b11);
    if (!(device->bars[bir].present)) {
        print("ERROR: BAR used for MSIX vector table not present");
        while(1);
    }
    MSIX_TableEntry * table = (MSIX_TableEntry*)(device->bars[bir].base_address + (msix_capability->Table & ~0b11));
    uint64_t* pba = (uint64_t*)(device->bars[bir].base_address + (msix_capability->Pending_Bit & ~0b11));
    int table_size = (int)(0x7ff & msix_capability->Message_Control) + 1;

    int lapic_id = 0x0;
    // fill the vector table
    for (int i = 0; i < table_size; i++) {
        table[i].Message_Address_Low = 0xfee00000 | (lapic_id << 12);
        table[i].Message_Address_High = 0x0;
        table[i].Message_Data = irq;
        table[i].Vector_Control = 0x1;
    }
    // enable MSIX
    msix_capability->Message_Control |= (1 << 15);

    for (int i = 0; i < table_size; i++) {
        table[i].Vector_Control = 0x0;
    }
    // unmask the function
    msix_capability->Message_Control &= ~(uint16_t)(1 << 14);
    

}

void __attribute__((optimize("O0"))) dump_MSI_capability(uint32_t* msi_capability) {
    
}

//TODO: handle 64 Bit vs 32 Bit addresses
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

}

