#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"
#include "../../src/include/apic.h"
#include "../../src/include/cpaging.h"
#include "../../src/include/acpi.h"


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



uint32_t* capability(PCIHeaderType0* device, int id) {
    if ((device->Status & (1 << 4)) >> 4) {
        uint32_t* cap =  (uint32_t*)((device->Capabilities_Pointer & 0b11111100) + (uint64_t)device);
        uint32_t next_ptr = (cap[0] >> 8) & 0xff;
        while (next_ptr != 0) {
            if ((cap[0] & 0xff) == id) {
                return cap;
            }
            cap = (uint32_t *)(next_ptr + (uint64_t)device);
            next_ptr = (cap[0] >> 8) & 0xff;
        }
    }
    return 0x0;
}


void device0(PCIHeaderType0* device) {
    uint32_t* cap = capability(device, 0x11);
    printhex(device->Class_Code);
    printf(" ");
    printhex(device->Subclass);
    printf(" ");
    printhex(device->Prog_IF);
    if (cap) {
        printf(": MSI-X\n");
        enable_MSIX();
    } else {
        cap = capability(device, 0x05);
        if (cap) {
            printf(": MSI\n");
            device->Command |= 0b111;
            device->Command |= (1 << 10);
            enable_MSI(device, cap);
        } else {
            printf(": NAN\n");
        }
    } 
}

void analyze_device(PCIHeader* header) {
    if (header->Vendor_ID != 0xffff) {
        switch (header->Header_Type & 0x3) {
        case 0x0:
            device0(((PCIHeaderType0*)header));
            break;
        case 0x1:
            /* code */
            break;
        case 0x2:
            /* code */
            break;
        default:
            printf("ERROR");
            while(1);
            break;
        }
    } 
    return;
}


void parse_MCFG() {
    if (!(acpi.Flags & 0b010)) {
        printf("ERROR: No MCFG table present");
        while(1);
    }
    for (int offset = 44; offset < acpi.MCFG->Length; offset+=16) {
        MCFG_entry* entry = (MCFG_entry*)((uint64_t)acpi.MCFG + offset);
        uint64_t addr = entry->base_address;
        for (int bus = entry->start_PCI_bus_number; bus < entry->end_PCI_bus_number; bus++) {
            for (int device = 0; device < 32; device++) {
                for (int func = 0; func < 8; func++) {
                    PCIHeader* phy_addr = (PCIHeader*)(addr + (bus << 20 | device << 15 | func << 12));
                    analyze_device(phy_addr);
                }
            }
        }
    }

}