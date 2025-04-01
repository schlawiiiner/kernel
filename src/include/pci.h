#ifndef PCI_H
#define PCI_H
#include "../../src/include/uint.h"

typedef struct __attribute__((packed)) PCIHeader{
    uint16_t Vendor_ID;
    uint16_t Device_ID;
    uint16_t Command;
    uint16_t Status;
    uint8_t Revision_ID;
    uint8_t Prog_IF;
    uint8_t Subclass;
    uint8_t Class_Code;
    uint8_t Cache_Line_Size;
    uint8_t Latency_Timer;
    uint8_t Header_Type;
    uint8_t BIST;
    union {
        struct __attribute__((packed)) {
            uint32_t Base_Address_0;
            uint32_t Base_Address_1;
            uint32_t Base_Address_2;
            uint32_t Base_Address_3;
            uint32_t Base_Address_4;
            uint32_t Base_Address_5;
            uint32_t Cardbus_CIS_Pointer;
            uint16_t Subsystem_Vendor_ID;
            uint16_t Subsystem_ID;
            uint32_t Expansion_ROM_Base_Address;
            uint8_t Capabilities_Pointer;
            uint8_t Reserved_0[3];
            uint32_t Reserved_1;
            uint8_t Interrupt_Line;
            uint8_t Interrrupt_PIN;
            uint8_t Min_Grant;
            uint8_t Max_Latency;
        } Type0;
        struct __attribute__((packed)) {
            uint32_t Base_Address_0;
            uint32_t Base_Address_1;
            uint8_t Primary_Bus_Number;
            uint8_t Secondary_Bus_Number;
            uint8_t Subordinate_Bus_Number;
            uint8_t Secondary_Latency_Timer;
            uint8_t IO_Base;
            uint8_t IO_Limit;
            uint16_t Secondary_Status;
            uint16_t Memory_Base;
            uint16_t Memory_Limit;
            uint16_t Prefetchable_Memory_Base;
            uint16_t Prefetchable_Memory_Limit;
            uint32_t Prefetchable_Base_Upper_32_Bits;
            uint32_t Prefetchable_Limit_Upper_32_Bits;
            uint16_t IO_Base_Upper_16_Bits;
            uint16_t IO_Limit_Upper_16_Bits;
            uint8_t Capability_Pointer;
            uint8_t Reserved[3];
            uint32_t Expansion_ROM_Base_Address;
            uint8_t Interrupt_Line;
            uint8_t Interrupt_PIN;
            uint16_t Bridge_Control;
        } Type1;
        struct __attribute__((packed)) {
            uint32_t CardBus_Socket_ExCa_Base_Address;
            uint8_t Offset_Of_Capabilities_List;
            uint8_t Reserved;
            uint16_t Secondary_Status;
            uint8_t PCI_Bus_Number;
            uint8_t CardBus_Bus_Number;
            uint8_t Subordinate_Bus_Number;
            uint8_t CardBus_Latency_Timer;
            uint32_t Memory_Base_Address_0;
            uint32_t Memory_Limit_0;
            uint32_t Memory_Base_Address_1;
            uint32_t Memory_Limit_1;
            uint32_t IO_Base_Address_0;
            uint32_t IO_Limit_0;
            uint32_t IO_Base_Address_1;
            uint32_t IO_Limit_1;
            uint8_t Interrupt_Line;
            uint8_t Interrupt_PIN;
            uint16_t Bridge_Control;
            uint16_t Subsystem_Device_ID;
            uint16_t Subsystem_Vendor_ID;
            uint32_t _16_Bit_PC_Card_Legacy_Mode_Base_Address;
        } Type2;   
    } HeaderType;
} PCIHeader;

typedef struct __attribute__((packed)) PCIe_CAP {
    uint8_t ID;
    uint8_t next_CAP_ptr;
    uint16_t PCIe_CAP_REG;
    uint32_t Device_Capabilities;
    uint16_t Device_Control;
    uint16_t Device_Status;
    uint32_t Link_Capabilities;
    uint16_t Link_Control;
    uint16_t Link_Status;
    uint32_t Slot_Capabilities;
    uint16_t Slot_Control;
    uint16_t Slot_Status;
    uint16_t Root_Control;
    uint16_t Root_Capabilities;
    uint32_t Root_Status;
    uint32_t Device_Compatibilities_2;
    uint16_t Device_Control_2;
    uint16_t Device_Status_2;
    uint32_t Link_Capabilities_2;
    uint16_t Link_Control_2;
    uint16_t Link_Status_2;
} PCIe_CAP;

typedef struct __attribute__((packed)) PCI_Power_Management_CAP {
    uint8_t ID;
    uint8_t next_ptr;
    uint16_t PMC;       // Power Management Capabilities
    uint16_t PMCSR;     // Power Management Control/Status Register
    uint8_t PMCSR_BSE;  // Bridge Support Extension
    uint8_t Data;
} PCI_Power_Management_CAP;

typedef struct __attribute__((packed)) PCI_MSIX_CAP {
    uint8_t ID;
    uint8_t next_ptr;
    uint16_t Message_Control;
    uint32_t Table;
    uint32_t Pending_Bit;
} PCI_MSIX_CAP;



/*Command Register Bits*/
#define IO_SPACE                            1 << 0
#define MEMORY_SPACE                        1 << 1
#define BUS_MASTER                          1 << 2
#define SPECIAL_CYCLES                      1 << 3
#define MEMORY_WRITE_AND_INVALIDATE_ENABLE  1 << 4
#define VGA_PALETTE_SNOOP                   1 << 5
#define PARITY_ERROR_RESPONSE               1 << 6
#define STEPPING_CONTROL                    1 << 7
#define SERR_ENABLE                         1 << 8
#define FAST_BACK_TO_BACK_ENABLE            1 << 9

/*Device Status*/
#define CAPABILITIES_LIST                   1 << 4
#define _66MHZ_CAPABLE                      1 << 5
#define FAST_BACK_TO_BACK_CAPABLE           1 << 7
#define MASTER_DATA_PARITY_ERROR            1 << 8
#define DEVSEL_TIMING                       0b11 << 9
#define SIGNAL_TARGET_ABORT                 1 << 11
#define RECEIVED_TARGET_ABORT               1 << 12
#define RECEIVED_MASTER_ABORT               1 << 13
#define SIGNALED_SYSTEM_ERROR               1 << 14
#define DETECTED_PARITY_ERROR               1 << 15

/*
-----------------------------------------------------------
OS specific structures, generated during device enumeration
may be extended in future
-----------------------------------------------------------
*/
typedef struct __attribute__((packed)) PCI_BAR {
    uint64_t base_address;
    uint32_t size;
    uint8_t present;        //if zero BAR is not used else it is used
    uint8_t type;           //if zero IO space else MMIO
    uint8_t prefetchable;   //if zero not prefetchable else prefetchable
    uint8_t addr_range;     //if zero 32-Bit else 64-Bit BAR
} PCI_BAR;


typedef struct __attribute__((packed)) PCI_DEV {
    uint16_t vendor;
    uint16_t device;

    uint32_t revision;
    uint32_t class;

    uint8_t bus;
    uint8_t slot;
    uint8_t function;
    uint8_t hdr_type;

    PCIHeader* PCI_Config_Space;
    PCI_BAR bars[6];

    uint16_t irq;

    void*                       msi_cap;
    PCI_MSIX_CAP*               msix_cap;
    PCIe_CAP*                   pcie_cap;
    PCI_Power_Management_CAP*   pci_pmi_cap;

    void * driver_config_space;
} PCI_DEV;

typedef struct __attribute__((packed)) PCI_DEV_List {
    uint64_t number_devices;
    PCI_DEV devices[100];
} PCI_DEV_List;

extern volatile PCI_DEV_List device_list;

void enumerate_devices();
void dump_devices();
void dump_capability(volatile PCI_DEV* device);
volatile PCI_DEV* get_device(int id);
int get_device_number();
#endif