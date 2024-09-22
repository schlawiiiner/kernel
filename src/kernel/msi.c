#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"

void enable_MSIX() {

}

void __attribute__((optimize("O0"))) dump_MSI_capability(uint32_t* msi_capability) {
    for (int i = 0; i < 6; i++) {
        printbin(msi_capability[i]);
        printf("\n");
    }
}


void __attribute__((optimize("O0"))) enable_MSI(PCIHeaderType0* device, uint32_t* msi_capability) {
    int num_msi_vectors = ((msi_capability[0] >> 17) & 0b111) + 1;
    int capable_of_64_bit = ((msi_capability[0] >> 23) & 0b1);

    uint32_t msi_addr =  msi_capability[1];
    msi_addr &= 0b111111110000;
    msi_addr |= (uint32_t)(0xfee << 20);
    msi_capability[1] = msi_addr;

    uint32_t msi_data_low = msi_capability[3];
    msi_data_low &= ~0b1100011111111111;
    msi_data_low |= 0x23;
    msi_capability[3] = msi_data_low;
    msi_capability[4] = 0;
    msi_capability[5] = 0;
    msi_capability[0] |= (1 << 16);
}

