global device_list

section .bss
align 8
device_list:
    resb 8      ; number of devices
    resb 128*100 ; 100 supported devices each device has a (currently) 128 byte large data structure (see pci.h PCI_DEV)
