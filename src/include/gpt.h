#ifndef GPT_H
#define GPT
#include "../../src/include/uint.h"

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
    uint64_t Starting_LBA;
    uint32_t Partition_Number;
    uint32_t Partition_Entry_Size;
    uint32_t Checksum_Partition_Entries;
    uint8_t Reserved1[420];
} PartitionTableHeader;

#endif