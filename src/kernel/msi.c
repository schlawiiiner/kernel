#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"

void __attribute__((optimize("O0"))) enable_MSIX(int device_number) {
    PCI_DEV* device = &(device_list.devices[device_number]);
    if (!(device->msix_cap_offset)) {
        printf("ERROR: device ");
        printdec(device_number);
        printf(" does not support MSIX");
        while(1);
    }
    uint32_t* msix_capability = (uint32_t*)((uint64_t)(device->PCI_Config_Space) + device->msix_cap_offset);
}

void __attribute__((optimize("O0"))) dump_MSI_capability(uint32_t* msi_capability) {
    
}


void __attribute__((optimize("O0"))) enable_MSI(int device_number) {
    PCI_DEV* device = &(device_list.devices[device_number]);
    if (!(device->msi_cap_offset)) {
        printf("ERROR: device ");
        printdec(device_number);
        printf(" does not support MSI");
        while(1);
    }
    uint32_t* msi_capability = (uint32_t*)((uint64_t)(device->PCI_Config_Space) + device->msi_cap_offset);
}

