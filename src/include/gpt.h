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
}PartitionEntry;

#endif