# Device List
During the device enumeration the kernel generates a global `device_list` of all PCI devices found. This `device_list` has the folowing structure:
```c
struct PCI_DEV_LIST {
    uint64_t number_devices;
    PCI_DEV device[100];
}
```
`number_devices` stores the total number of devices. For each device there is made an entry in the `device` array, which has the following structure.

```c
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

    uint16_t msi_cap_offset;
    uint16_t msix_cap_offset;
    uint16_t pcie_cap_offset;

    uint64_t driver_config_space;
} PCI_DEV;

``` 
- `vendor`: vendor ID from PCI configuration space
- `device`: device ID from PCI configuration space
- `revision`: revision from PCI configuration space
- `class`: class code, subclass code and prog IF form PCI configuration space
- `bus`: bus where the device is attached to
- `slot`: slot where the device is attached
- `function`: for multi-function devices this indicates the function
- `hdr_type`: PCI header type of the device
- `PCI_Config_Space`: pointer to the PCI configuration space
- `bars`: array containing information for each BAR
- `irq`: IRQ number that is related to this device

...

- `driver_config_space`: A pointer to a memory location that the driver can use for the device configuration. (Its up to the driver to allocate memory. e.g could be used to store the dequeue pointer of a xhci event ring)