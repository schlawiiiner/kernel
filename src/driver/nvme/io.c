#include "../../../src/include/nvme.h"
#include "../../../src/include/mm/memory.h"
#include "../../../src/include/gpt.h"
#include "../../../src/include/ext4.h"
#include "../../../src/include/crc.h"
#include "../../../src/include/io.h"

#define LIMITED_RETRY           (1 << 31)
#define FORCE_UNIT_ACCESS       (1 << 30)
#define STORAGE_TAG_CHECK       (1 << 24)


#define CRC32_SEED              0x0

#define EXT4_NO_CHECKSUM        0x0
#define EXT4_CRC32C_CHECKSUM    0x1

#define SECTOR_SIZE              0x200

uint16_t read_command(volatile PCI_DEV* device, int y, uint64_t buffer, uint64_t metadata, uint64_t slba, uint16_t nlb, uint32_t nsid) {
    if (y < 0) {
        print("ERROR: invalid Queue ID");
        while(1);
    } else if (y != 1) {
        print("ERROR: currently only on I/O Queue implemented");
        while(1);
    }
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_SubmissionQueueEntry* cmd = cs->Queues[y].SQE + cs->Queues[y].SQT;


    cmd->Opcode = 0x02;
    cmd->Flags = 0x0;
    cmd->Command_ID = pop_cid(cs);
    cmd->NSID = nsid;
    cmd->Metadata = metadata;
    set_prp(cmd, (void*)buffer, nlb*SECTOR_SIZE);
    cmd->CDW2 = 0x0;
    cmd->CDW3 = 0x0;
    cmd->CDW[0] = (uint32_t)slba;
    cmd->CDW[1] = (uint32_t)(slba << 32);
    cmd->CDW[2] = nlb;
    cmd->CDW[3] = 0x0;
    cmd->CDW[4] = 0x0;
    cmd->CDW[5] = 0x0;

    ring_submission_queue_tail_doorbell(device, y);
    return cmd->Command_ID;
}

uint32_t gpt_header_checksum(PartitionTableHeader* header) {
    uint32_t checksum_copy = header->Checksum;
    header->Checksum = 0; // Zero out checksum field
    uint32_t crc = calculate_crc32(header, 0x5C, CRC32_SEED);
    header->Checksum = checksum_copy; // Restore original checksum
    return crc;
}

void read_inode_table(volatile PCI_DEV* device, Superblock* superblock, BlockGroupDescriptor* descr) {
    uint64_t slba = ((uint64_t)descr->inode_table_hi << 32) | (descr->inode_table_lo);
    uint64_t buffer_size = superblock->inode_size*superblock->inodes_per_group;
    uint64_t buffer = malloc(buffer_size);
    uint16_t cid = read_command(device, 1, buffer, 0x0, slba, buffer_size/SECTOR_SIZE, 0x1);
    poll_cq(device, cid);

    for (uint64_t inode_ptr = buffer; inode_ptr < inode_ptr + buffer_size; inode_ptr+=superblock->inode_size) {
        Inode* inode = (Inode*)inode_ptr;
        printhex(inode->block[0]);
    }
}

void read_block_group_descriptors(volatile PCI_DEV* device, PartitionEntry* partition, Superblock* superblock) {
    if (1 == superblock->first_data_block) {
        print("ERROR: Does not support 1024 Byte Block Size");
        return;
    }
    uint64_t block_group_count = ((uint64_t)superblock->blocks_count_hi << 32 | superblock->blocks_count_lo)/superblock->blocks_per_group;
    uint64_t slba = partition->Starting_LBA + (1024 << superblock->log_block_size) / SECTOR_SIZE;
    uint64_t size = block_group_count*sizeof(BlockGroupDescriptor);
    uint64_t nlba = (size + SECTOR_SIZE - 1)/SECTOR_SIZE;
    BlockGroupDescriptor* block_group_descr = (BlockGroupDescriptor*)malloc(size);
    memset((uint64_t)block_group_descr, 0x0, size);
    uint16_t cid = read_command(device, 1, (uint64_t)block_group_descr, 0x0, slba, nlba, 0x1);
    poll_cq(device, cid);

    for (uint64_t i = 0; i < block_group_count; i++) {
        read_inode_table(device, superblock, block_group_descr + i);
    }
}

void mount_filesystem(volatile PCI_DEV* device, PartitionEntry* partition) {
    uint16_t nlb = sizeof(Superblock)/SECTOR_SIZE;
    Superblock* superblock = (Superblock*)malloc(sizeof(Superblock));
    memset((uint64_t)superblock, 0x0, sizeof(Superblock));
    uint64_t slba = partition->Starting_LBA + SUPERBLOCK_OFFSET;
    uint16_t cid = read_command(device, 1, (uint64_t)superblock, 0x0, slba, nlb, 0x1);
    poll_cq(device, cid);
    if (superblock->magic != MAGIC) {
        print("ERROR: Magic Number wrong\n");
        return;
    }
    if (superblock->checksum_type != EXT4_CRC32C_CHECKSUM) {
        print("ERROR: Unknown Checksum Type\n");
        return;
    }
    if (superblock->checksum != (0xffffffff - calculate_crc32c(superblock, sizeof(Superblock)-sizeof(superblock->checksum), CRC32_SEED))) {
        print("ERROR: Wrong Checksum\n");
        return;
    }
    read_block_group_descriptors(device, partition, superblock);
    free((uint64_t)superblock, sizeof(Superblock));
}

void read_partition_table(volatile PCI_DEV* device, PartitionTableHeader* header) {
    int n = header->Partition_Number;
    uint64_t size = n*header->Partition_Entry_Size;
    uint16_t nlb = size/512;
    uint64_t buffer = malloc(size);
    memset(buffer, 0x0, size);
    uint16_t cid = read_command(device, 1, buffer, 0x1, header->Starting_LBA_Partition_Table, nlb, 0x1); //TODO: if nlb instead of 0x1 then ERROR Status
    poll_cq(device, cid);

    if (header->Checksum_Partition_Entries != calculate_crc32((void*)buffer, size, CRC32_SEED)) {
        print("ERROR: Invalid Checksum");
        while(1);
    }
    PartitionEntry* partition_table = (PartitionEntry*)buffer;
    for (int i = 0; i < n; i++) {
        // Declare a temporary variable to store the GUID
        uint64_t guid_low = partition_table[i].Partition_Type_GUID[0];
        uint64_t guid_high = partition_table[i].Partition_Type_GUID[1];
        if ((guid_low == UNUSED) && (guid_high == UNUSED)) {
            continue;
        } else if ((guid_low == EFI_SYSTEM_LOW) && (guid_high == EFI_SYSTEM_HIGH)) {
            print("EFI System\n");
        } else if ((guid_low == MICROSOFT_RESERVED_LOW) && (guid_high == MICROSOFT_RESERVED_HIGH)) {
            print("Microsoft Reserved\n");
        } else if ((guid_low == MICROSOFT_BASIC_DATA_LOW) && (guid_high == MICROSOFT_BASIC_DATA_HIGH)) {
            print("Microsoft Basic Data\n");
        } else if ((guid_low == LINUX_FILESYSTEM_LOW) && (guid_high == LINUX_FILESYSTEM_HIGH)) {
            print("Linux Filesystem\n");
            mount_filesystem(device, partition_table + i);
        } else if ((guid_low == WINDOWS_RE_LOW) && (guid_high == WINDOWS_RE_HIGH)) {
            print("Windows RE\n");
        } else {
            print("Unknown\n");
        }   
    }
    free(buffer, size);
}

void test_read(volatile PCI_DEV* device) {
    uint64_t buffer = malloc(0x1000);
    uint64_t metadata = malloc(0x1000);
    uint16_t cid =  read_command(device, 1, buffer, metadata, 0x1, 0x0, 0x1);
    poll_cq(device, cid);
    PartitionTableHeader* header = (PartitionTableHeader*)buffer;
    if (header->Signature != GPT_SIGNATURE) {
        print("ERROR: GPT Signature wrong");
        while(1);
    }
    if (header->Checksum != gpt_header_checksum(header)) {
        print("ERROR: Invalid checksum");
        while(1);
    }
    read_partition_table(device, header);
    return;
}