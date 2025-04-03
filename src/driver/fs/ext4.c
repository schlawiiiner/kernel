#include "../../../src/include/ext4.h"
#include "../../../src/include/crc.h"
#include "../../../src/include/nvme.h"
#include "../../../src/include/mm/memory.h"

void read_inode(volatile PCI_DEV* device, PartitionEntry* partition, Superblock* superblock, uint32_t inode_num) {
    uint32_t block_group_number = (inode_num - 1) / superblock->inodes_per_group;
    uint32_t inode_index = (inode_num - 1) % superblock->inodes_per_group;

    uint64_t block_group_offset = (1024 << superblock->log_block_size) + block_group_number*sizeof(BlockGroupDescriptor);
    uint64_t bg_slba = partition->Starting_LBA + block_group_offset/SECTOR_SIZE;
    uint64_t offset = block_group_offset % SECTOR_SIZE;
    uint64_t buffer = malloc(SECTOR_SIZE);
    BlockGroupDescriptor* descr = (BlockGroupDescriptor*)(buffer + offset);
    uint16_t cid = read_command(device, 1, buffer, 0x0, bg_slba, 0x0, 0x1);
    poll_cq(device, cid);

    uint64_t i_table_block = (((uint64_t)descr->inode_table_hi << 32) | (descr->inode_table_lo));
    uint64_t i_offset = inode_index*superblock->inode_size;
    uint64_t i_slba = ((i_table_block * (1024 << superblock->log_block_size) + i_offset)/SECTOR_SIZE) + partition->Starting_LBA;
    uint64_t i_buffer = malloc(SECTOR_SIZE);
    Inode* i = (Inode*)(i_buffer + i_offset);
    uint16_t i_cid = read_command(device, 1, i_buffer, 0x0, i_slba, 0x0, 0x1);
    poll_cq(device, i_cid);

    printhex(i->mode);
    print("\n");
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
    read_inode(device, partition, superblock, 2);
    //read_block_group_descriptors(device, partition, superblock);
    free((uint64_t)superblock, sizeof(Superblock));
}