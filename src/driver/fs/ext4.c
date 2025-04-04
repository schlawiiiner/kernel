#include "../../../src/include/ext4.h"
#include "../../../src/include/crc.h"
#include "../../../src/include/nvme.h"
#include "../../../src/include/mm/memory.h"
#include "../../../src/include/io.h"

#define EXT4_ICACHE_SIZE                0x200

InodeCache* icache_l1[EXT4_ICACHE_SIZE] __attribute__((section(".sysvar")));
InodeCache* icache_l2[EXT4_ICACHE_SIZE] __attribute__((section(".sysvar")));


int hash_inode(uint64_t inode_number) {
    return inode_number % EXT4_ICACHE_SIZE;
}

InodeCache* cache_inode(Inode* inode, uint64_t inode_number) {
    InodeCache* inode_cache = (InodeCache*)malloc(sizeof(InodeCache));
    inode_cache->inode_number = inode_number;
    inode_cache->used = 0x0;
    inode_cache->dirty = 0x0;
    inode_cache->mode = inode->mode;
    inode_cache->uid = ((uint32_t)inode->osd2.linux2.l_uid_high << 16) | inode->uid;
    inode_cache->gid = ((uint32_t)inode->osd2.linux2.l_gid_high << 16) | inode->gid;
    inode_cache->link_count = inode->links_count;
    inode_cache->block_count = ((uint64_t)inode->osd2.linux2.l_blocks_high << 32) | inode->blocks_lo;
    inode_cache->flags = inode->flags;
    for (int i = 0; i < 15; i++) {
        inode_cache->block_ptr[i] = inode->block[i];
    }

    int index = hash_inode(inode_number);
    if (icache_l1[index]) {
        if (icache_l2[index]->used) {
            print("ERROR: Inode is beeing accessed right now");
            while(1);
        }
        free(icache_l2[index], sizeof(InodeCache));
        icache_l2[index] = icache_l1[index];
    }
    icache_l1[index] = inode_cache;
    return inode_cache;
}

InodeCache* fetch_inode(volatile PCI_DEV* device, PartitionEntry* partition, Superblock* superblock, uint64_t inode_number) {
    int index = hash_inode(inode_number);

    InodeCache* inode_cached = (InodeCache*)0x0;

    /*Try to read Inode from cache*/

    if (icache_l1[index]) {
        if (icache_l1[index]->inode_number == inode_number) {
            inode_cached = icache_l1[index];
            inode_cached->used++;
            return inode_cached;
        } else if (icache_l2[index]) {
            if (icache_l2[index]->inode_number == inode_number) {
                inode_cached = icache_l2[index];
                inode_cached->used++;
                return inode_cached;
            }
        }
    }

    /*Now we need to fetch the inode from the Disk*/

    
    uint32_t block_group_index = (inode_number - 1) / superblock->inodes_per_group;
    uint32_t inode_index = (inode_number - 1) % superblock->inodes_per_group;

    /*Read Block Group Descriptor*/

    uint64_t block_group_offset = (1024 << superblock->log_block_size) + block_group_index*sizeof(BlockGroupDescriptor);
    uint64_t block_group_slba = partition->Starting_LBA + block_group_offset/SECTOR_SIZE;
    uint64_t offset = block_group_offset % SECTOR_SIZE;
    uint64_t buffer = malloc(SECTOR_SIZE);
    uint16_t cid = read_command(device, 1, (uint64_t)buffer, 0x0, block_group_slba, 0x0, 0x1);
    poll_cq(device, cid);
    BlockGroupDescriptor* descr = (BlockGroupDescriptor*)(buffer + offset);

    /*Read Inode*/

    uint64_t inode_table = (((uint64_t)descr->inode_table_hi << 32) | (descr->inode_table_lo));
    uint64_t inode_offset = inode_index*superblock->inode_size;
    uint64_t inode_slba = ((inode_table * (1024 << superblock->log_block_size) + inode_offset)/SECTOR_SIZE) + partition->Starting_LBA;
    uint64_t inode_offset_offset = ((inode_table * (1024 << superblock->log_block_size) + inode_offset) % SECTOR_SIZE);

    uint64_t inode_buffer = malloc(SECTOR_SIZE);
    uint16_t inode_cid = read_command(device, 1, (uint64_t)inode_buffer, 0x0, inode_slba, 0x0, 0x1);
    poll_cq(device, inode_cid);
    Inode* inode = (Inode*)(inode_buffer + inode_offset_offset);

    free(buffer, SECTOR_SIZE);
    free(inode_buffer, SECTOR_SIZE);

    return cache_inode(inode, inode_number);
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
    InodeCache* inode = fetch_inode(device, partition, superblock, 2);
    printhex(inode->mode);
    print("\n");
    free((uint64_t)superblock, sizeof(Superblock));
}