#ifndef XHCI_H
#define XHCI_H
#include "../../src/include/uint.h"
#include "../../src/include/pci.h"

/*Capability Registers*/
#define CPLENGTH    0x00 	//Capability Register Length
#define RSVD        0x01 	//Reserved
#define HCIVERSION  0x02 	//Interface Version Number
#define HCSPARAMS1  0x04 	//Structural Parameters 1
#define HCSPARAMS2  0x08 	//Structural Parameters 2
#define HCSPARAMS3  0x0C 	//Structural Parameters 3
#define HCCPARAMS1  0x10 	//Capability Parameters
#define DBOFF       0x14 	//Doorbell Offset
#define RTSOFF      0x18 	//Runtime Registers Space Offset
#define HCCPARMS2   0x1C 	//Capability Parameters 2 

/*Operational Registers*/
#define USBCMD      0x00 	//USB Command
#define USBSTS      0x04 	//USB Status
#define PAGESIZE    0x08 	//Page Size
#define DNCTRL      0x14 	//Device Notification Control
#define CRCR        0x18 	//Command Ring Control
#define DCBAAP      0x30 	//Device Context Base Address Array Pointer
#define CONFIG      0x38 	//Configure

/*Port Registers*/
#define PORTSC      0x00 	//Port Status and Control
#define PORTPMSC    0x04 	//Port Power Management Status and Control
#define PORTLI      0x08 	//Port Link Info

/*Runtime Registers*/
#define MFINDEX     0x00 	//Microframe Index
#define IRO         0x20 	//Interrupter Register Sets 

#define IMAN        0x00 	//Interrupter Management
#define IMOD        0x04 	//Interrupter Moderation
#define ERSTSZ      0x08 	//Event Ring Segment Table Size
#define ERSTBA      0x10 	//Event Ring Segment Table Base Address
#define ERDP        0x18 	//Event Ring Dequeue Pointer 

void init_xhci_controller(PCIHeaderType0* xhci_device);
#endif