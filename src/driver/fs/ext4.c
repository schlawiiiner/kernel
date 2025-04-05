#include "../../../src/include/ext4.h"
#include "../../../src/include/crc.h"
#include "../../../src/include/nvme.h"
#include "../../../src/include/mm/memory.h"
#include "../../../src/include/io.h"

#define EXT4_ICACHE_SIZE                0x200

InodeCache* icache_l1[EXT4_ICACHE_SIZE] __attribute__((section(".sysvar")));
InodeCache* icache_l2[EXT4_ICACHE_SIZE] __attribute__((section(".sysvar")));

void read_lba(volatile PCI_DEV* device, uint64_t slba, uint64_t nlba, void* buffer) {
    uint16_t cid = read_command(device, 1, (uint64_t)buffer, 0x0, slba, nlba, 0x1);
    poll_cq(device, cid);
}

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
    inode_cache->generation = inode->generation;
    for (int i = 0; i < 15; i++) {
        inode_cache->block_ptr[i] = inode->block[i];
    }

    int index = hash_inode(inode_number);
    if (icache_l1[index]) {
        if (icache_l2[index]->used) {
            print("ERROR: Inode is beeing accessed right now");
            while(1);
        }
        free((uint64_t)icache_l2[index], sizeof(InodeCache));
        icache_l2[index] = icache_l1[index];
    }
    icache_l1[index] = inode_cache;
    return inode_cache;
}

InodeCache* fetch_inode(ext4_Filesystem* fs, uint64_t inode_number) {
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

    
    uint32_t block_group_index = (inode_number - 1) / fs->inodes_per_group;
    uint32_t inode_index = (inode_number - 1) % fs->inodes_per_group;

    /*Read Block Group Descriptor*/

    uint64_t block_group_offset = fs->block_size + block_group_index*sizeof(BlockGroupDescriptor);
    uint64_t block_group_slba = fs->slba + block_group_offset/SECTOR_SIZE;
    uint64_t offset = block_group_offset % SECTOR_SIZE;
    uint64_t buffer = malloc(SECTOR_SIZE);
    read_lba(fs->device, block_group_slba, 0x0, (void*)buffer);
    BlockGroupDescriptor* descr = (BlockGroupDescriptor*)(buffer + offset);

    /*Read Inode*/

    uint64_t inode_table = (((uint64_t)descr->inode_table_hi << 32) | (descr->inode_table_lo));
    uint64_t inode_offset = inode_index*fs->inodes_size;
    uint64_t inode_slba = ((inode_table * fs->block_size + inode_offset)/SECTOR_SIZE) + fs->slba;
    uint64_t inode_offset_offset = ((inode_table * fs->block_size + inode_offset) % SECTOR_SIZE);

    uint64_t inode_buffer = malloc(SECTOR_SIZE);
    read_lba(fs->device, inode_slba, 0x0, (void*)inode_buffer);
    Inode* inode = (Inode*)(inode_buffer + inode_offset_offset);

    free(buffer, SECTOR_SIZE);
    free(inode_buffer, SECTOR_SIZE);

    return cache_inode(inode, inode_number);
}


DirectoryEntry* next_dir_entry(DirectoryEntry* dir) {
    return (DirectoryEntry*)((uint64_t)dir + dir->length);
}


int is_absolute_path(char* path) {
    if (path[0] == '/') {
        return 0x1;
    } else {
        return 0x0;
    }
}

int dir_name_length(char* path) {
    int i = 0;
    while(1) {
        if (path[i] == '\0') {
            //invalid
            return 0x0;
        } else if (path[i] == '/') {
            return i;
        }
        i++;
    }
}

char* get_filename(char* path) {
    int i = 0;
    char* name = 0x0;
    while(1) {
        if (*(path + i) == '/') {
            name = path + i + 1;
        } else if (*(path + i) == '\0') {
            return name;
        }
        i++;
    }
}

InodeCache* find_inode(ext4_Filesystem* fs, InodeCache* inode, char* name) {
    if (!(inode->mode & 0x4000)) {
        print("ERROR:\tNo Directory");
        while(1);
    }
    if (!(inode->flags & 0x80000)) {
        print("ERROR:\tExtents Flag not set");
        while(1);
    }
    ExtentHeader* eh = (ExtentHeader*)(inode->block_ptr);
    if (eh->magic != EH_MAGIC) {
        print("ERROR:\tExtents Magic wrong");
        while(1);
    }
    if (eh->depth == 0) {
        Extent* ee = (Extent*)((uint64_t)eh + sizeof(ExtentHeader));
        uint64_t block = ((uint64_t)ee->start_hi << 32) | ee->start_lo;
        uint64_t sector = block_to_sector(fs, block);
        uint64_t buffer = malloc(fs->block_size);
        read_lba(fs->device, sector, fs->sectors_per_block - 1, (void*)buffer);
        DirectoryEntry* entry = (DirectoryEntry*)buffer;
        for (int i = 0; i < (inode->link_count + 10); i++) {
            if (!entry->inode) {
                break;
            }
            if (compare_string(entry->name, name)) {
                return fetch_inode(fs, entry->inode);
            }
            if (entry->length > 253) {
                // This 'should'? indicate the end of the directory
                break;
            }
            entry = next_dir_entry(entry);
        }
        free(buffer, fs->block_size);
    }
    return (InodeCache*)0x0;
}

uint32_t dir_entries_checksum(ext4_Filesystem* fs, InodeCache* inode, void* dir_block) {
    uint32_t crc = calculate_crc32c(&inode->inode_number, 4, fs->seed);
    crc = calculate_crc32c(&inode->generation, 4, crc);
    crc = calculate_crc32c(dir_block, fs->block_size - sizeof(DirectoryEntryTail), crc);
    return crc;
} 

void list_directories(ext4_Filesystem* fs, InodeCache* inode) {
    if (!(inode->mode & 0x4000)) {
        print("ERROR:\tNo Directory");
        while(1);
    }
    if (!(inode->flags & 0x80000)) {
        print("ERROR:\tExtents Flag not set");
        while(1);
    }
    ExtentHeader* eh = (ExtentHeader*)(inode->block_ptr);
    if (eh->magic != EH_MAGIC) {
        print("ERROR:\tExtents Magic wrong");
        while(1);
    }
    if (eh->depth == 0) {
        Extent* ee = (Extent*)((uint64_t)eh + sizeof(ExtentHeader));
        uint64_t block = ((uint64_t)ee->start_hi << 32) | ee->start_lo;
        uint64_t sector = block_to_sector(fs, block);
        uint64_t buffer = malloc(fs->block_size);
        read_lba(fs->device, sector, fs->sectors_per_block - 1, (void*)buffer);
        DirectoryEntry* dir = (DirectoryEntry*)buffer;
        print("\n");
        uint64_t buffer_end = buffer + fs->block_size;
        while((buffer_end - (uint64_t)dir) > sizeof(DirectoryEntryTail)) {
            if (!dir->inode) {
                //unused entry
                dir = next_dir_entry(dir);
                continue;
            }
            print(dir->name);
            print("\t\t");
            dir = next_dir_entry(dir);
        }
        print("\n\n");

        /*Check Directory Entry Tail*/

        DirectoryEntryTail* tail = (DirectoryEntryTail*)dir;
        if ((buffer_end - (uint64_t)dir) != sizeof(DirectoryEntryTail)) {
            print("ERROR:\tDirectory Entry Tail not alligned to correctly\n");
        }
        if (tail->inode_zero != 0) {
            print("ERROR:\tDirectory Entry Tail Inode not zero\n");
        }
        if (tail->lenght != sizeof(DirectoryEntryTail)) {
            print("ERROR:\tDirectory Entry Tail length not 12 bytes\n");
        }
        if (tail->name_len_zero != 0) {
            print("ERROR:\tDirectory Entry Tail Lenght of file name not zero\n");
        }
        if (tail->file_type_reserved != DET_FILE_TYPE) {
            print("ERROR:\tDirectory Entry Tail file type not 0xDE\n");
        }
        if (tail->checksum != dir_entries_checksum(fs, inode, (void*)buffer)) {
            print("ERROR:\tDirectory leaf block checksum wrong\n");
        }
        printhex(sizeof(Inode));
        free(buffer, fs->block_size);
    }
}

InodeCache* change_directory(ext4_Filesystem* fs, InodeCache* inode, char* path) {
    int i = 0;
    if (is_absolute_path(path)) {
        inode = fetch_inode(fs, 2);
        i++;
    }
    while(1) {
        if (path[i] == '\0') {
            return inode;
        }
        int len = dir_name_length(path+i);
        if (!len) {
            print("ERROR:\tInvalid Path\n");
            return (InodeCache*)0x0;
        }
        path[len+i] = '\0';
        inode = find_inode(fs, inode, path+i);
        path[len+i] = '/';
        i+=len+1;
    }    
}


File* load_file(ext4_Filesystem* fs, InodeCache* inode, char* path) {
    char* filename = get_filename(path);
    char copy = *filename;
    *filename = '\0';
    InodeCache* dir = change_directory(fs, inode, path);
    if (!dir) {
        print("ERROR:\tPath does not exist");
        while(1);
    }
    *filename = copy;
    InodeCache* file = find_inode(fs, dir, filename);
    if (!file) {
        print("ERROR:\tFile does not exist");
        while(1);
    }
    if (!(file->mode & 0x8000)) {
        print("ERROR:\tFile is no Regular File");
        while(1);
    }
    if (!(file->flags & 0x80000)) {
        print("ERROR:\tExtents Flag not set");
        while(1);
    }
    ExtentHeader* eh = (ExtentHeader*)(file->block_ptr);
    if (eh->magic != EH_MAGIC) {
        print("ERROR:\tExtents Magic wrong");
        while(1);
    }
    if (eh->depth == 0) {
        Extent* ee = (Extent*)((uint64_t)eh + sizeof(ExtentHeader));
        uint64_t block = ((uint64_t)ee->start_hi << 32) | ee->start_lo;
        uint64_t sector = block_to_sector(fs, block);
        uint64_t buffer = malloc(fs->block_size);
        read_lba(fs->device, sector, fs->block_size/fs->sector_size - 1, (void*)buffer);

        File* file_data = (File*)malloc(sizeof(File));
        file_data->buffer = (void*)buffer;
        file_data->size = fs->block_size;
        return file_data;
    }

    return (File*)0x0;
}

ext4_Filesystem* init_filesystem(volatile PCI_DEV* device, PartitionEntry* partition, Superblock* superblock) {
    ext4_Filesystem* fs = (ext4_Filesystem*)malloc(sizeof(ext4_Filesystem));
    fs->device = device;
    fs->block_size = 1024 << superblock->log_block_size;
    fs->sector_size = SECTOR_SIZE;
    fs->inodes_per_group = superblock->inodes_per_group;
    fs->inodes_size = superblock->inode_size;
    fs->sectors_per_block = fs->block_size/fs->sector_size;
    fs->slba = partition->Starting_LBA;
    fs->seed = calculate_crc32c(superblock->uuid, 16, CRC32_SEED);
    return fs;
}

int supports_features(uint32_t features) {
    uint32_t driver_features = EXT4_FEATURE_INCOMPAT_FILETYPE | EXT4_FEATURE_INCOMPAT_EXTENTS | EXT4_FEATURE_INCOMPAT_64BIT;
    return features & ~driver_features;
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
    if (superblock->checksum != calculate_crc32c(superblock, sizeof(Superblock)-sizeof(superblock->checksum), CRC32_SEED)) {
        print("ERROR: Wrong Checksum\n");
        return;
    }
    /*if (supports_features(superblock->feature_incompat)) {
        print("ERROR:\tFeatrues are not supported\n");
        printbin(supports_features(superblock->feature_incompat));
        print("\n");
        printbin(superblock->feature_incompat);
        print("\n");
        return;
    }*/

    ext4_Filesystem* fs = init_filesystem(device, partition, superblock);
    list_directories(fs, fetch_inode(fs, 2));
    File* file = load_file(fs, (InodeCache*)0x0, "/bin/main.elf");
    uint8_t* buffer = file->buffer;
    free((uint64_t)fs, sizeof(ext4_Filesystem));
    free((uint64_t)superblock, sizeof(Superblock));
}