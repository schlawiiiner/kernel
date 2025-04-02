#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"
#include "../../src/include/apic.h"
#include "../../src/include/mm/memory.h"
#include "../../src/include/acpi.h"
#include "../../src/include/io.h"

volatile PCI_DEV_List device_list __attribute__((section(".sysvar")));

void add_capabilities(volatile PCI_DEV* device) {
    PCIHeader* header = device->PCI_Config_Space;
    if (!(header->Status & CAPABILITIES_LIST)) {
        return;
    }
    uint8_t* cap_ptr;
    switch (device->hdr_type) {
    case 0x0:
        cap_ptr = (uint8_t*)header + (((PCIHeader*)header)->HeaderType.Type0.Capabilities_Pointer & ~3);
        break;
    case 0x1:
        cap_ptr =  (uint8_t*)header + (((PCIHeader*)header)->HeaderType.Type1.Capability_Pointer & ~3);
        break;
    case 0x2:
        cap_ptr = (uint8_t*)header + (((PCIHeader*)header)->HeaderType.Type2.Offset_Of_Capabilities_List & ~3);
        break;
    default:
        print("unknown Header Type ");
        printdec(device->hdr_type);
        print("\n");
        return;
    }
    for (int i = 0; i < 256 && cap_ptr; i++) {
        uint8_t cap_id = *cap_ptr;
        switch (cap_id) {
        case 0x1:
            device->pci_pmi_cap = (PCI_Power_Management_CAP*)cap_ptr;
            break;
        case 0x5:
            device->msi_cap = (void*)cap_ptr;
            break;
        case 0x10:
            device->pcie_cap = (PCIe_CAP*)cap_ptr;
            break;
        case 0x11:
            device->msix_cap = (PCI_MSIX_CAP*)cap_ptr;
            break;
        default:
            //TODO: other ID's must to be implemented and added to the PCI_DEV struct
            break;
        }
        uint8_t next_ptr = *(cap_ptr + 1);
        cap_ptr = next_ptr ? ((uint8_t*)header + (next_ptr & ~3)) : 0x0;
    }
}

void dump_capability(volatile PCI_DEV* device) {
        print("\n ------- PCI Capabilities -------\n");
    if (device->pci_pmi_cap) {
        print("  PCI Power Management Interface\n");
    }
    if (device->pcie_cap) {
        print("  PCI Express\n");
    }
    if (device->msi_cap) {
        print("  MSI\n");
    }
    if (device->msix_cap) {
        print("  MSI-X\n");
    }
    print(" --------------------------------\n");
}

void map_32_BAR(uint32_t* bar_ptr, int bar_number, int device_number) {
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
            map_to(page_addr, page_addr, n_pages*PAGE_SIZE, 0x0);
        } else {
            map_to(page_addr, page_addr, n_pages*PAGE_SIZE, WRITE_THROUGH | CACHE_DISABLE);
        }

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

void map_IO_BAR(uint32_t* bar_ptr, int bar_number, int device_number) {
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

void map_64_BAR(uint32_t* bar_ptr, int bar_number, int device_number) {
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
        int cache_disable = 1;
        if (bara & 0b1000) {
            //prefetchable
            cache_disable = 0;
            map_to(page_addr, page_addr, n_pages*PAGE_SIZE, 0x0);
        } else {
            map_to(page_addr, page_addr, n_pages*PAGE_SIZE, WRITE_THROUGH | CACHE_DISABLE);
        }
        for (int i = 0; i < 2; i++) {
            device_list.devices[device_number].bars[bar_number+i].base_address = addr;
            device_list.devices[device_number].bars[bar_number+i].addr_range = 0x1;
            device_list.devices[device_number].bars[bar_number+i].prefetchable = (uint8_t)!cache_disable;
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

void map_device(PCIHeader* device, int device_number) {
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
                print("ERROR: Invalid BAR");
                while(1);
                break;
            }
        }
    }
}

/*This function adds the device to the global device list*/
void add_device(PCIHeader* device, int bus, int slot, int func) {
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
        map_device((PCIHeader*)device, n);
    }

    device_list.devices[n].PCI_Config_Space = device;
    
    device_list.devices[n].irq = 0;
    
    add_capabilities(&device_list.devices[n]);

    device_list.number_devices++;
}

void dump_device(int id) {
    if (id >= device_list.number_devices) {
        print("Invalid device id");
        return;
    }
    print("-------------------------------------------\n");
    print("Vendor ID: ");
    printhex(device_list.devices[id].vendor);
    print("\nDevice ID: ");
    printhex(device_list.devices[id].device);
    print("\nClass    : ");
    printhex(device_list.devices[id].class);
    print("\n-------------------------------------------\n\n");

    write_string_to_serial("-------------------------------------------\n");
    write_string_to_serial("Vendor ID: ");
    write_hex_to_serial(device_list.devices[id].vendor);
    write_string_to_serial("\nDevice ID: ");
    write_hex_to_serial(device_list.devices[id].device);
    write_string_to_serial("\nClass    : ");
    write_hex_to_serial(device_list.devices[id].class);
    write_string_to_serial("\n-------------------------------------------\n\n");
}

volatile PCI_DEV* get_device(int id) {
    if (id >= device_list.number_devices) {
        print("Invalid device id");
        return (volatile PCI_DEV*)0x0;
    }
    return &device_list.devices[id];
}

int get_device_number() {
    return device_list.number_devices;
}

void dump_devices() {
    for (uint64_t i = 0; i < device_list.number_devices; i++) {
        dump_device(i);
    }
}

void check_device(MCFG_entry* entry, int bus, int slot) {
    int func = 0;
    PCIHeader* device = (PCIHeader*)((uint64_t)(entry->base_address) + (bus << 20 | slot << 15 | func << 12));
    if (device->Vendor_ID == 0xffff) {
        return;
    }
    if (device->Header_Type & (1 << 7)) {
        for (func = 1; func < 8; func++) {
            PCIHeader* device = (PCIHeader*)((uint64_t)(entry->base_address) + (bus << 20 | slot << 15 | func << 12));
            if (device->Vendor_ID == 0xffff) {
                continue;
            } else {
                add_device(device, bus, slot, func);
            }
        }
    } else {
        add_device(device, bus, slot, func);
    }
}

void enumerate_devices() {
    if (!(acpi.Flags & 0b010)) {
        print("ERROR: MCFG table not present");
        while(1);
    }
    device_list.number_devices = 0;
    for (uint32_t offset = 44; offset < acpi.MCFG->Length; offset+=sizeof(MCFG_entry)) {
        MCFG_entry* entry = (MCFG_entry*)((uint64_t)acpi.MCFG + offset);
        uint64_t addr = entry->base_address;
        for (int bus = entry->start_PCI_bus_number; bus <= entry->end_PCI_bus_number; bus++) {
            for (int slot = 0; slot < 32; slot++) {
                check_device(entry, bus, slot);
            }
        }
    }
    printhex(device_list.number_devices);
}