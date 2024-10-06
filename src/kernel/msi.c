#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"

void __attribute__((optimize("O0"))) enable_MSIX(int device_number) {
    volatile PCI_DEV* device = &(device_list.devices[device_number]);
    //check if the device supports MSIX
    if (!(device->msix_cap_offset)) {
        print("ERROR: device ");
        printdec(device_number);
        print(" does not support MSIX");
        while(1);
    }
    
    uint32_t* msix_capability = (uint32_t*)((uint64_t)(device->PCI_Config_Space) + device->msix_cap_offset);
    int bir = (int)(msix_capability[1] & 0b11);
    if (!(device->bars[bir].present)) {
        print("ERROR: BAR used for MSIX vector table not present");
        while(1);
    }

    uint32_t * table = (uint32_t*)(device->bars[bir].base_address + (msix_capability[1] & ~0b11));
    int table_size = (int)(msix_capability[0] >> 16) + 1;

    // fill the vector table
    for (int i = 0; i < 4*table_size; i+=4) {
        table[i] = (table[i] & 0xfffff) | 0xfee00000;
        table[i+1] = 0x0;
        table[i+2] = (table[i+2] & 0xffffff00) | 0x23;
        table[i+3] &= ~(uint32_t)0b1;
    }
    // enable MSIX
    msix_capability[0] |= (1 << 31);
    // unmask the function
    msix_capability[0] &= ~(uint32_t)(1 << 30);
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

