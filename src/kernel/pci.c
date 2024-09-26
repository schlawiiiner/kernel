#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"
#include "../../src/include/apic.h"
#include "../../src/include/cpaging.h"
#include "../../src/include/acpi.h"

/*Returns the offset of the capability with ID id in the PCI configuration space, if not present returns zero as offset*/
uint16_t capability(PCIHeader* device, int id) {
    //check if capability list is implemented
    if ((device->Status & (1 << 4)) >> 4) {
        uint32_t* cap;
        if (device->Header_Type == 0x0) {
            cap =  (uint32_t*)((((PCIHeaderType0 *)device)->Capabilities_Pointer & 0b11111100) + (uint64_t)device);
        } else if (device->Header_Type == 0x1) {
            cap =  (uint32_t*)((((PCIHeaderType1 *)device)->Capability_Pointer & 0b11111100) + (uint64_t)device);
        } else {
            cap =  (uint32_t*)((((PCIHeaderType2 *)device)->Offset_Of_Capabilities_List & 0b11111100) + (uint64_t)device);
        }
        uint16_t next_ptr = (cap[0] >> 8) & 0xff;
        while (next_ptr != 0) {
            if ((cap[0] & 0xff) == id) {
                return (uint16_t)((uint64_t)cap - (uint64_t)device);
            }
            cap = (uint32_t *)(next_ptr + (uint64_t)device);
            next_ptr = (cap[0] >> 8) & 0xff;
        }
    }
    return 0x0;
}

void dump_capability(PCIHeader* device) {
    if (device->Header_Type != 0) {
        printf(" nan\n");
        return;
    }
    if ((device->Status & (1 << 4)) >> 4) {
        uint32_t* cap = (uint32_t*)((((PCIHeaderType0 *)device)->Capabilities_Pointer & 0b11111100) + (uint64_t)device);
        uint16_t next_ptr = (cap[0] >> 8) & 0xff;
        while (next_ptr != 0) {
            printhex(cap[0] & 0xff);
            printf(" ");
            cap = (uint32_t *)(next_ptr + (uint64_t)device);
            next_ptr = (cap[0] >> 8) & 0xff;
        }
    }
    printf(" nan\n");
}

void __attribute__((optimize("O0"))) map_32_BAR(uint32_t* bar_ptr, int bar_number, int device_number) {
    uint32_t bar = bar_ptr[0];
    bar_ptr[0] = 0xffffffff;
    uint32_t size = bar_ptr[0] & 0xFFFFFFF0;
    bar_ptr[0] = bar;
    if (size) {
        //BAR is used
        uint64_t addr = (uint64_t)(bar & 0xfffffff0);
        uint64_t offset = addr % PAGE_SIZE;
        uint64_t page_addr = addr - offset;
        size = ~size + 1 + offset;
        int n_pages = size / PAGE_SIZE;
        if (size % PAGE_SIZE) {
            n_pages++;
        }
        int cache_disable = 1;
        if (bar & 0b1000) {
            //prefetchable
            cache_disable = 0;
        }
        io_map(page_addr, n_pages, 1, 0, cache_disable, cache_disable);

        device_list.devices[device_number].bars[bar_number].base_address = addr;
        device_list.devices[device_number].bars[bar_number].addr_range = 0x0;
        device_list.devices[device_number].bars[bar_number].prefetchable = (uint8_t)!cache_disable;
        device_list.devices[device_number].bars[bar_number].present = 0x1;
        device_list.devices[device_number].bars[bar_number].size = size;
        device_list.devices[device_number].bars[bar_number].type = 0x1;
    } else {
        //BAR is not used
        device_list.devices[device_number].bars[bar_number].present = 0x0;
    }
}

void __attribute__((optimize("O0"))) map_IO_BAR(uint32_t* bar_ptr, int bar_number, int device_number) {
    uint32_t bar = bar_ptr[0];
    bar_ptr[0] = 0xffffffff;
    uint32_t size = (uint32_t)(bar_ptr[0] & 0xfffffffc);
    bar_ptr[0] = bar;
    if (size) {
        //BAR is used
        device_list.devices[device_number].bars[bar_number].base_address = bar & (~(uint32_t)0b1);
        device_list.devices[device_number].bars[bar_number].present = 0x1;
        device_list.devices[device_number].bars[bar_number].size = ~size+1;
        device_list.devices[device_number].bars[bar_number].type = 0x0;
    } else {
        //BAR is not used
        device_list.devices[device_number].bars[bar_number].present = 0x0;
    }
}

void __attribute__((optimize("O0"))) map_64_BAR(uint32_t* bar_ptr, int bar_number, int device_number) {
    uint32_t bara = bar_ptr[0];
    uint32_t barb = bar_ptr[1];
    bar_ptr[0] = 0xffffffff;
    bar_ptr[1] = 0xffffffff;
    uint64_t size = ((uint64_t)bar_ptr[1] << 32) | (uint64_t)(bar_ptr[0] & 0xFFFFFFF0);
    bar_ptr[0] = bara; 
    bar_ptr[1] = barb;
    if (size) {
        //BAR is used
        uint64_t addr = ((uint64_t)barb << 32)| (uint64_t)(bara & 0xfffffff0);
        uint64_t offset = addr % PAGE_SIZE;
        uint64_t page_addr = addr - offset;
        size = ~size + 1 + offset;
        int n_pages = size / PAGE_SIZE;
        if (size % PAGE_SIZE) {
            n_pages++;
        }
        int cd = 1; //cache disable
        if (bara & 0b1000) {
            //prefetchable
            cd = 0;
        }
        io_map(page_addr, n_pages, 1, 0, cd, cd);

        for (int i = 0; i < 2; i++) {
            device_list.devices[device_number].bars[bar_number+i].base_address = addr;
            device_list.devices[device_number].bars[bar_number+i].addr_range = 0x1;
            device_list.devices[device_number].bars[bar_number+i].prefetchable = (uint8_t)!cd;
            device_list.devices[device_number].bars[bar_number+i].present = 0x1;
            device_list.devices[device_number].bars[bar_number+i].size = size;
            device_list.devices[device_number].bars[bar_number+i].type = 0x1;
        }
    } else {
        //BAR is not used
        device_list.devices[device_number].bars[bar_number].present = 0x0;
        device_list.devices[device_number].bars[bar_number+1].present = 0x0;
    }
}

void map_device(PCIHeaderType0* device, int device_number) {
    uint32_t *barx = (uint32_t*)((uint64_t)device + 0x10);
    int x = 0;
    while (x < 6) {
        if (barx[x] & 0b1) {
            //IO
            x+=1;
        } else {
            //MMIO
            switch (barx[x] & 0b110) {
            case 0b000: {
                //32 Bit
                map_32_BAR(barx+x, x, device_number);
                x+=1;
                break;
            }
            case 0b100: {
                //64 Bit
                map_64_BAR(barx+x, x, device_number);
                x+=2;
                break;
            }
            default:
                //ERROR
                printf("ERROR: Invalid BAR");
                while(1);
                break;
            }
        }
    }
}

/*This function adds the device to the global device list*/
void add_device(MCFG_entry* entry, int bus, int slot, int func) {
    PCIHeader* device = (PCIHeader*)((uint64_t)(entry->base_address) + (bus << 20 | slot << 15 | func << 12));
    if (device->Vendor_ID == 0xffff) {
        return;
    }
    int n = device_list.number_devices;
    device_list.devices[n].vendor = device->Vendor_ID;
    device_list.devices[n].device = device->Device_ID;

    device_list.devices[n].revision = (uint32_t)device->Revision_ID;
    device_list.devices[n].class = ((uint32_t)(device->Class_Code) << 16) | ((uint32_t)(device->Subclass) << 8) | (uint32_t)(device->Prog_IF);

    device_list.devices[n].bus = bus;
    device_list.devices[n].slot = slot;
    device_list.devices[n].function = func;
    device_list.devices[n].hdr_type = device->Header_Type & 0b01111111;
    if (device_list.devices[n].hdr_type == 0x0) {
        map_device((PCIHeaderType0*)device, n);
    }

    device_list.devices[n].PCI_Config_Space = device;
    
    device_list.devices[n].irq = 0;

    device_list.devices[n].msi_cap_offset = capability(device, 0x5);
    device_list.devices[n].msix_cap_offset = capability(device, 0x11);
    device_list.devices[n].pcie_cap_offset = capability(device, 0x10);

    device_list.number_devices++;
}

void dump_device(int id) {
    printf("-------------------------------------------\n");
    printf("Vendor ID: ");
    printhex(device_list.devices[id].vendor);
    printf("\nDevice ID: ");
    printhex(device_list.devices[id].device);
    printf("\nClass    : ");
    printhex(device_list.devices[id].class);
    printf("\n-------------------------------------------\n\n");
}
void dump_devices() {
    for (int i = 0; i < device_list.number_devices; i++) {
        dump_device(i);
    }
}

void enumerate_devices() {
    if (!(acpi.Flags & 0b010)) {
        printf("ERROR: MCFG table not present");
        while(1);
    }
    device_list.number_devices = 0;
    for (int offset = 44; offset < acpi.MCFG->Length; offset+=sizeof(MCFG_entry)) {
        MCFG_entry* entry = (MCFG_entry*)((uint64_t)acpi.MCFG + offset);
        uint64_t addr = entry->base_address;
        for (int bus = entry->start_PCI_bus_number; bus <= entry->end_PCI_bus_number; bus++) {
            for (int slot = 0; slot < 32; slot++) {
                for (int func = 0; func < 8; func++) {
                    add_device(entry, bus, slot, func);
                }
            }
        }
    }
}