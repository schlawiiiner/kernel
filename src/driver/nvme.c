#include "../../src/include/nvme.h"
#include "../../src/include/mm/memory.h"

void disable(ControllerProperties* cp) {
    cp->CC = (cp->CC) & (~(uint32_t)0x1);
    int timeout = 0;
    while ((cp->CSTS & 0x1)) {
        if (timeout > 0x1000000) {
            print("timeout\n");
            break;
        }
        timeout++;
    }
    
}

void allocate_ASQ(ControllerProperties* cp) {
    uint64_t asq = malloc(0x1000);
    uint64_t asq_paddr = get_paddr(asq);
}

void allocate_ACQ(ControllerProperties* cp) {

}

void init_nvme_controller(volatile PCI_DEV* nvme_controller) {
    if (nvme_controller->class != 0x010802) {
        return;
    }
    dump_capability(nvme_controller);
    if (0x0 == nvme_controller->pcie_cap_offset) {
        print("nvme controller does not support pcie\n");
        return;
    }
    ControllerProperties* cp = (ControllerProperties*)(nvme_controller->bars[0].base_address);
    disable(cp);
    allocate_ASQ(cp);
    return;
}

