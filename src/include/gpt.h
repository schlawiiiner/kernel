#ifndef GPT_H
#define GPT
#include "../../src/include/uint.h"

#define GPT_SIGNATURE           0x5452415020494645

typedef struct __attribute__((packed)) PartitionTableHeader {
    uint64_t Signature;
    uint32_t Revision;
    uint32_t Header_size;
    uint32_t Checksum;
    uint32_t Reserved;
    uint64_t Current_LBA;
    uint64_t Backup_LBA;
    uint64_t First_usabel_LBA;
    uint64_t Last_usable_LBA;
    uint64_t GUID[2];
    uint64_t Starting_LBA_Partition_Table;
    uint32_t Partition_Number;
    uint32_t Partition_Entry_Size;
    uint32_t Checksum_Partition_Entries;
    uint8_t Reserved1[420];
} PartitionTableHeader;

typedef struct __attribute__((packed)) PartitionEntry {
    uint64_t Partition_Type_GUID[2];
    uint64_t Unique_Partition_GUID[2];
    uint64_t Starting_LBA;
    uint64_t Ending_LBA;
    uint64_t Attributes;
    char Partition_Name[72];
} PartitionEntry;

#define UNUSED                      0x0

#define EFI_SYSTEM_LOW              0x11d2f81fc12a7328
#define EFI_SYSTEM_HIGH             0x3bc93ec9a0004bba
#define MICROSOFT_RESERVED_LOW      0x4db80b5ce3c9e316
#define MICROSOFT_RESERVED_HIGH     0xae1502f02df97d81
#define MICROSOFT_BASIC_DATA_LOW    0x4433b9e5ebd0a0a2
#define MICROSOFT_BASIC_DATA_HIGH   0xc79926b7b668c087
#define LINUX_FILESYSTEM_LOW        0x477284830fc63daf
#define LINUX_FILESYSTEM_HIGH       0xe47d47d8693d798e
#define WINDOWS_RE_LOW              0x4d4006d1de94bba4
#define WINDOWS_RE_HIGH             0xacd67901d5bf6aa1
//TODO: More GUID

#endif