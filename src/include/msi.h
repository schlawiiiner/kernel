#ifndef MSI_H
#define MSI_H
#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"

void enable_MSIX();
void __attribute__((optimize("O0"))) enable_MSI(PCIHeaderType0* device, uint32_t* msi_capability);
#endif