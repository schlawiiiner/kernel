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

static inline uint8_t read_CPLENGTH(uint64_t capability_registers) {
   return ((uint8_t*)(capability_registers + CPLENGTH))[0];
}

static inline uint16_t read_HCIVERSION(uint64_t capability_registers) {
   return ((uint16_t*)(capability_registers + HCIVERSION))[0];
}

static inline uint32_t read_HCSPARAMS1(uint64_t capability_registers) {
   return ((uint32_t*)(capability_registers + HCSPARAMS1))[0];
}

static inline uint32_t read_HCSPARAMS2(uint64_t capability_registers) {
   return ((uint32_t*)(capability_registers + HCSPARAMS2))[0];
}

static inline uint32_t read_HCSPARAMS3(uint64_t capability_registers) {
   return ((uint32_t*)(capability_registers + HCSPARAMS3))[0];
}

static inline uint32_t read_HCCPARAMS1(uint64_t capability_registers) {
   return ((uint32_t*)(capability_registers + HCCPARAMS1))[0];
}

static inline uint32_t read_DBOFF(uint64_t capability_registers) {
   return ((uint32_t*)(capability_registers + DBOFF))[0];
}

static inline uint32_t read_RTSOFF(uint64_t capability_registers) {
   return ((uint32_t*)(capability_registers + RTSOFF))[0];
}

static inline uint32_t read_HCCPARAMS2(uint64_t capability_registers) {
   return ((uint32_t*)(capability_registers + HCCPARMS2))[0];
}

/*Command Registers*/

static inline uint32_t read_USBCMD(uint64_t operational_registers) {
   return ((uint32_t*)(operational_registers + USBCMD))[0];
}

static inline uint32_t read_USBSTS(uint64_t operational_registers) {
   return ((uint32_t*)(operational_registers + USBSTS))[0];
}

static inline uint32_t read_PAGESIZE(uint64_t operational_registers) {
   return ((uint32_t*)(operational_registers + PAGESIZE))[0];
}

static inline uint32_t read_DNCTRL(uint64_t operational_registers) {
   return ((uint32_t*)(operational_registers + DNCTRL))[0];
}

static inline uint64_t read_CRCR(uint64_t operational_registers) {
   return (((uint64_t)((uint32_t*)(operational_registers +  CRCR + 0x04))[0]) << 32) | ((uint32_t*)(operational_registers +  CRCR))[0];
}

static inline uint64_t read_DCBAAP(uint64_t operational_registers) {
   return (((uint64_t)((uint32_t*)(operational_registers +  DCBAAP + 0x04))[0]) << 32) | ((uint32_t*)(operational_registers +  DCBAAP))[0];
}

static inline uint32_t read_CONFIG(uint64_t operational_registers) {
   return ((uint32_t*)(operational_registers + CONFIG))[0];
}





static inline void  write_USBCMD(uint64_t operational_registers, uint32_t value) {
   ((uint32_t*)(operational_registers + USBCMD))[0] = value;
}

static inline void  write_USBSTS(uint64_t operational_registers, uint32_t value) {
   ((uint32_t*)(operational_registers + USBSTS))[0] = value;
}

static inline void  write_PAGESIZE(uint64_t operational_registers, uint32_t value) {
   ((uint32_t*)(operational_registers + PAGESIZE))[0] = value;
}

static inline void  write_DNCTRL(uint64_t operational_registers, uint32_t value) {
   ((uint32_t*)(operational_registers + DNCTRL))[0] = value;
}

static inline void  write_CRCR(uint64_t operational_registers, uint64_t value) {
   ((uint32_t*)(operational_registers + CRCR))[0] = (uint32_t)value;
   ((uint32_t*)(operational_registers + CRCR + 0x4))[0] = (uint32_t)(value >> 32);
}

static inline void  write_DCBAAP(uint64_t operational_registers, uint64_t value) {
   ((uint32_t*)(operational_registers + DCBAAP))[0] = (uint32_t)value;
   ((uint32_t*)(operational_registers + DCBAAP + 0x4))[0] = (uint32_t)(value >> 32);
}

static inline void  write_CONFIG(uint64_t operational_registers, uint32_t value) {
   ((uint32_t*)(operational_registers + CONFIG))[0] = value;
}


void init_xhci_controller(int device_number);
#endif