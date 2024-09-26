global device_list

section .bss
align 8
device_list:
    resb 8          ; number of devices
    resb 136*100    ; 100 supported devices each device has a (currently) 136 byte large data structure (see pci.h PCI_DEV)
