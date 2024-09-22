#include "../../src/include/uint.h"
#include "../../src/include/pci.h"
#include "../../src/include/xhci.h"
#include "../../src/include/graphics.h"
#include "../../src/include/cpaging.h"

void init_xhci_controller(PCIHeaderType0* xhci_device) {
    printf("\nbeginning xhci initialization...\n");
    uint64_t capability_registers = ((uint64_t)xhci_device->Base_Address_1 << 32) | (uint64_t)(xhci_device->Base_Address_0);
    identity_map(capability_registers, 1, 1, 0, 0, 0);
    uint64_t operational_registers = capability_registers + ((uint32_t*)(capability_registers + CPLENGTH))[0];
    identity_map(operational_registers, 1, 1, 0, 0, 0);
    uint32_t page_size = ((uint32_t*)(operational_registers + PAGESIZE))[0];
    printbin(page_size);
}