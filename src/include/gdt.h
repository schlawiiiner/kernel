#ifndef GDT_H
#define GDT_H
#include "../../src/include/uint.h"

/*
Access Byte (CODE-/DATA SEGMENT)
*/
#define GDT_PRESENT                 1 << 7
#define DPL_RING0                   0b00 << 5
#define DPL_RING1                   0b01 << 5
#define DPL_RING2                   0b10 << 5
#define DPL_RING3                   0b11 << 5
#define SEGMENT_DESCRIPTOR          1 << 4
#define EXECUTABLE                  1 << 3
#define DIRECTION                   1 << 2
#define READABLE_WRITABLE           1 << 1
#define ACCESSED                    1 << 0
/*
Access Byte (TSS/LDT)
*/
#define LDT_TYPE                    0x2
#define TSS_AVAILABLE_TYPE          0x9
#define TSS_BUSY_TYPE               0xb                     


/*
FLAGS
*/
#define LONG_MODE                   1 << 1
#define SIZE                        1 << 2
#define GRANULARITY                 1 << 3




typedef struct __attribute__((packed, aligned(8))) SegmentDescriptor {
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    uint8_t AccessByte;
    uint8_t Limit1_Flags;
    uint8_t Base2;
} SegmentDescriptor;

typedef struct __attribute__((packed, aligned(8))) SystemSegmentDescriptor {
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    uint8_t AccessByte;
    uint8_t Limit1_Flags;
    uint8_t Base2;
    uint32_t Base3;
    uint32_t Reserved;
} SystemSegmentDescriptor;

typedef struct __attribute__((packed, aligned(8))) GlobalDescriptorTable {
    SegmentDescriptor Null;
    SegmentDescriptor KernelCode;
    SegmentDescriptor KernelData;
    SegmentDescriptor UserCode;
    SegmentDescriptor UserData;
    SystemSegmentDescriptor TSS[];
} GlobalDescriptorTable;

typedef struct __attribute__((packed, aligned(4))) GDTR {
    uint16_t Size;      //GDT size - 1
    GlobalDescriptorTable* GDTP;
} GDTR;

typedef struct __attribute__((packed, aligned(8))) TaskStateSegment {
    uint32_t Resvd0;
    uint64_t RSP0;
    uint64_t RSP1;
    uint64_t RSP2;
    uint64_t Resvd1;
    uint64_t IST1;
    uint64_t IST2;
    uint64_t IST3;
    uint64_t IST4;
    uint64_t IST5;
    uint64_t IST6;
    uint64_t IST7;
    uint64_t Resvd2;
    uint16_t Resvd3;
    uint16_t IOPB;
} TaskStateSegment;

#endif