#ifndef EXT4_H
#define EXT4_H
#include "../../src/include/uint.h"
#include "../../src/include/pci.h"
#include "../../src/include/gpt.h"

#define MAGIC							0xef53
#define SUPERBLOCK_OFFSET				0x2

#define EXT4_FEATURE_COMPAT_DIR_PREALLOC	0x0001
#define EXT4_FEATURE_COMPAT_IMAGIC_INODES	0x0002
#define EXT4_FEATURE_COMPAT_HAS_JOURNAL		0x0004
#define EXT4_FEATURE_COMPAT_EXT_ATTR		0x0008
#define EXT4_FEATURE_COMPAT_RESIZE_INODE	0x0010
#define EXT4_FEATURE_COMPAT_DIR_INDEX		0x0020
#define EXT4_FEATURE_COMPAT_SPARSE_SUPER2	0x0200
#define EXT4_FEATURE_COMPAT_FAST_COMMIT		0x0400
#define EXT4_FEATURE_COMPAT_STABLE_INODES	0x0800
#define EXT4_FEATURE_COMPAT_ORPHAN_FILE		0x1000	/* Orphan file exists */

#define EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER	0x0001
#define EXT4_FEATURE_RO_COMPAT_LARGE_FILE	0x0002
#define EXT4_FEATURE_RO_COMPAT_BTREE_DIR	0x0004
#define EXT4_FEATURE_RO_COMPAT_HUGE_FILE    0x0008
#define EXT4_FEATURE_RO_COMPAT_GDT_CSUM		0x0010
#define EXT4_FEATURE_RO_COMPAT_DIR_NLINK	0x0020
#define EXT4_FEATURE_RO_COMPAT_EXTRA_ISIZE	0x0040
#define EXT4_FEATURE_RO_COMPAT_QUOTA		0x0100
#define EXT4_FEATURE_RO_COMPAT_BIGALLOC		0x0200
#define EXT4_FEATURE_RO_COMPAT_METADATA_CSUM	0x0400
#define EXT4_FEATURE_RO_COMPAT_READONLY			0x1000
#define EXT4_FEATURE_RO_COMPAT_PROJECT			0x2000
#define EXT4_FEATURE_RO_COMPAT_VERITY			0x8000
#define EXT4_FEATURE_RO_COMPAT_ORPHAN_PRESENT	0x10000 /* Orphan file may be non-empty */

#define EXT4_FEATURE_INCOMPAT_COMPRESSION		0x0001
#define EXT4_FEATURE_INCOMPAT_FILETYPE			0x0002
#define EXT4_FEATURE_INCOMPAT_RECOVER			0x0004 /* Needs recovery */
#define EXT4_FEATURE_INCOMPAT_JOURNAL_DEV		0x0008 /* Journal device */
#define EXT4_FEATURE_INCOMPAT_META_BG			0x0010
#define EXT4_FEATURE_INCOMPAT_EXTENTS			0x0040 /* extents support */
#define EXT4_FEATURE_INCOMPAT_64BIT				0x0080
#define EXT4_FEATURE_INCOMPAT_MMP               0x0100
#define EXT4_FEATURE_INCOMPAT_FLEX_BG			0x0200
#define EXT4_FEATURE_INCOMPAT_EA_INODE			0x0400 /* EA in inode */
#define EXT4_FEATURE_INCOMPAT_DIRDATA			0x1000 /* data in dirent */
#define EXT4_FEATURE_INCOMPAT_CSUM_SEED			0x2000
#define EXT4_FEATURE_INCOMPAT_LARGEDIR			0x4000 /* >2GB or 3-lvl htree */
#define EXT4_FEATURE_INCOMPAT_INLINE_DATA		0x8000 /* data in inode */
#define EXT4_FEATURE_INCOMPAT_ENCRYPT			0x10000
#define EXT4_FEATURE_INCOMPAT_CASEFOLD			0x20000



#define EXT4_NO_CHECKSUM        0x0
#define EXT4_CRC32C_CHECKSUM    0x1

typedef struct __attribute__((packed)) Superblock {
    uint32_t	inodes_count;		    /* Inodes count */
	uint32_t	blocks_count_lo;	    /* Blocks count */
	uint32_t	r_blocks_count_lo;	    /* Reserved blocks count */
	uint32_t	free_blocks_count_lo;	/* Free blocks count */
	uint32_t	free_inodes_count;	    /* Free inodes count */
	uint32_t	first_data_block;	    /* First Data Block */
	uint32_t	log_block_size;	        /* Block size */
	uint32_t	log_cluster_size;	    /* Allocation cluster size */
	uint32_t	blocks_per_group;	    /* # Blocks per group */
	uint32_t	clusters_per_group;	    /* # Clusters per group */
	uint32_t	inodes_per_group;	    /* # Inodes per group */
	uint32_t	mtime;		            /* Mount time */
	uint32_t	wtime;		            /* Write time */
	uint16_t	mnt_count;		        /* Mount count */
	uint16_t	max_mnt_count;	        /* Maximal mount count */
	uint16_t	magic;		            /* Magic signature */
	uint16_t	state;		            /* File system state */
	uint16_t	errors;		            /* Behaviour when detecting errors */
	uint16_t	minor_rev_level;	    /* minor revision level */
	uint32_t	lastcheck;		        /* time of last check */
	uint32_t	checkinterval;	        /* max. time between checks */
	uint32_t	creator_os;		        /* OS */
	uint32_t	rev_level;		        /* Revision level */
	uint16_t	def_resuid;		        /* Default uid for reserved blocks */
	uint16_t	def_resgid;		        /* Default gid for reserved blocks */
	uint32_t	first_ino;		        /* First non-reserved inode */
	uint16_t    inode_size;		        /* size of inode structure */
	uint16_t	block_group_nr;	        /* block group # of this superblock */
	uint32_t	feature_compat;	        /* compatible feature set */
	uint32_t	feature_incompat;	    /* incompatible feature set */
	uint32_t	feature_ro_compat;	    /* readonly-compatible feature set */
	uint8_t	    uuid[16];		        /* 128-bit uuid for volume */
	char	    volume_name[16];        /* volume name */
	char	    last_mounted[64];	    /* directory where last mounted */
	uint32_t	algorithm_usage_bitmap; /* For compression */
	uint8_t	    prealloc_blocks;	    /* Nr of blocks to try to preallocate*/
	uint8_t	    prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	uint16_t	reserved_gdt_blocks;	/* Per group desc for online growth */
	uint8_t	    journal_uuid[16];	    /* uuid of journal superblock */
	uint32_t	journal_inum;		    /* inode number of journal file */
	uint32_t	journal_dev;		    /* device number of journal file */
	uint32_t	last_orphan;		    /* start of list of inodes to delete */
	uint32_t	hash_seed[4];		    /* HTREE hash seed */
	uint8_t	    def_hash_version;	    /* Default hash version to use */
	uint8_t	    jnl_backup_type;
	uint16_t    desc_size;		        /* size of group descriptor */
	uint32_t	default_mount_opts;
	uint32_t	first_meta_bg;	        /* First metablock block group */
	uint32_t	mkfs_time;		        /* When the filesystem was created */
	uint32_t	jnl_blocks[17];	        /* Backup of the journal inode */
    uint32_t	blocks_count_hi;	    /* Blocks count */
	uint32_t	r_blocks_count_hi;	    /* Reserved blocks count */
	uint32_t	free_blocks_count_hi;	/* Free blocks count */
	uint16_t	min_extra_isize;	    /* All inodes have at least # bytes */
	uint16_t	want_extra_isize; 	    /* New inodes should reserve # bytes */
	uint32_t	flags;		            /* Miscellaneous flags */
	uint16_t    raid_stride;		    /* RAID stride */
	uint16_t    mmp_update_interval;    /* # seconds to wait in MMP checking */
	uint64_t    mmp_block;              /* Block for multi-mount protection */
	uint32_t    raid_stripe_width;      /* blocks on all data disks (N*stride)*/
	uint8_t	    log_groups_per_flex;    /* FLEX_BG group size */
	uint8_t	    checksum_type;	        /* metadata checksum algorithm used */
	uint8_t	    encryption_level;	    /* versioning level for encryption */
	uint8_t	    reserved_pad;		    /* Padding to next 32bits */
	uint64_t	kbytes_written;	        /* nr of lifetime kilobytes written */
	uint32_t	snapshot_inum;	        /* Inode number of active snapshot */
	uint32_t	snapshot_id;		    /* sequential ID of active snapshot */
	uint64_t	snapshot_r_blocks_count; /* reserved blocks for active snapshot's future use */
	uint32_t	snapshot_list;	        /* inode number of the head of the on-disk snapshot list */
	uint32_t	error_count;		    /* number of fs errors */
	uint32_t	first_error_time;	    /* first time an error happened */
	uint32_t	first_error_ino;	    /* inode involved in first error */
	uint64_t	first_error_block;	    /* block involved of first error */
	uint8_t	    first_error_func[32];	/* function where the error happened */
	uint32_t	first_error_line;	    /* line number where error happened */
	uint32_t	last_error_time;	    /* most recent time of an error */
	uint32_t	last_error_ino;	        /* inode involved in last error */
	uint32_t	last_error_line;	    /* line number where error happened */
	uint64_t	last_error_block;	    /* block involved of last error */
	uint8_t	    last_error_func[32];	/* function where the error happened */
	uint8_t	    mount_opts[64];
	uint32_t	usr_quota_inum;	        /* inode for tracking user quota */
	uint32_t	grp_quota_inum;	        /* inode for tracking group quota */
	uint32_t	overhead_clusters;	    /* overhead blocks/clusters in fs */
	uint32_t	backup_bgs[2];	        /* groups with sparse_super2 SBs */
	uint8_t	    encrypt_algos[4];	    /* Encryption algorithms in use  */
	uint8_t	    encrypt_pw_salt[16];	/* Salt used for string2key algorithm */
	uint32_t	lpf_ino;		        /* Location of the lost+found inode */
	uint32_t	prj_quota_inum;	        /* inode for tracking project quota */
	uint32_t	checksum_seed;	        /* crc32c(uuid) if csum_seed set */
	uint8_t	    wtime_hi;
	uint8_t	    mtime_hi;
	uint8_t	    mkfs_time_hi;
	uint8_t	    lastcheck_hi;
	uint8_t	    first_error_time_hi;
	uint8_t	    last_error_time_hi;
	uint8_t	    first_error_errcode;
	uint8_t     last_error_errcode;
	uint16_t    encoding;		        /* Filename charset encoding */
	uint16_t    encoding_flags;	        /* Filename charset encoding flags */
	uint32_t    orphan_file_inum;	    /* Inode for tracking orphan inodes */
	uint32_t	reserved[94];		    /* Padding to the end of the block */
	uint32_t	checksum;		        /* crc32c(superblock) */
} Superblock;

typedef struct __attribute__((packed)) BlockGroupDescriptor {
	uint32_t	block_bitmap_lo;		/* Blocks bitmap block */
	uint32_t	inode_bitmap_lo;		/* Inodes bitmap block */
	uint32_t	inode_table_lo;			/* Inodes table block */
	uint16_t	free_blocks_count_lo;	/* Free blocks count */
	uint16_t	free_inodes_count_lo;	/* Free inodes count */
	uint16_t	used_dirs_count_lo;		/* Directories count */
	uint16_t	flags;					/* EXT4_BG_flags (INODE_UNINIT, etc) */
	uint32_t  	exclude_bitmap_lo;   	/* Exclude bitmap for snapshots */
	uint16_t  	block_bitmap_csum_lo;	/* crc32c(s_uuid+grp_num+bbitmap) LE */
	uint16_t  	inode_bitmap_csum_lo;	/* crc32c(s_uuid+grp_num+ibitmap) LE */
	uint16_t  	itable_unused_lo;		/* Unused inodes count */
	uint16_t  	checksum;				/* crc16(sb_uuid+group+desc) */
	/*64-bit support*/
	uint32_t	block_bitmap_hi;		/* Blocks bitmap block MSB */
	uint32_t	inode_bitmap_hi;		/* Inodes bitmap block MSB */
	uint32_t	inode_table_hi;			/* Inodes table block MSB */
	uint16_t	free_blocks_count_hi;	/* Free blocks count MSB */
	uint16_t	free_inodes_count_hi;	/* Free inodes count MSB */
	uint16_t	used_dirs_count_hi;		/* Directories count MSB */
	uint16_t  	itable_unused_hi;    	/* Unused inodes count MSB */
	uint32_t  	exclude_bitmap_hi;   	/* Exclude bitmap block MSB */
	uint16_t  	block_bitmap_csum_hi;	/* crc32c(s_uuid+grp_num+bbitmap) BE */
	uint16_t  	inode_bitmap_csum_hi;	/* crc32c(s_uuid+grp_num+ibitmap) BE */
	uint32_t   	reserved;
} BlockGroupDescriptor;

typedef struct __attribute__((packed)) Inode {
	uint16_t				mode;			/* File mode */
	uint16_t				uid;			/* Low 16 bits of Owner Uid */
	uint32_t				size_lo;		/* Size in bytes */
	uint32_t				atime;			/* Access time */
	uint32_t				ctime;			/* Inode Change time */
	uint32_t				mtime;			/* Modification time */
	uint32_t				dtime;			/* Deletion Time */
	uint16_t				gid;			/* Low 16 bits of Group Id */
	uint16_t				links_count;	/* Links count */
	uint32_t				blocks_lo;		/* Blocks count */
	uint32_t				flags;			/* File flags */
	union {
		struct {
			uint32_t  		l_version;
		} linux1;
		struct {
			uint32_t  		h_translator;
		} hurd1;
		struct {
			uint32_t  		m_reserved1;
		} masix1;
	} osd1;									/* OS dependent 1 */
	uint32_t				block[15];		/* Pointers to blocks */
	uint32_t				generation;		/* File version (for NFS) */
	uint32_t				file_acl_lo;	/* File ACL */
	uint32_t				size_high;
	uint32_t				obso_faddr;		/* Obsoleted fragment address */
	union {
		struct {
			uint16_t		l_blocks_high; 	/* were i_reserved1 */
			uint16_t		l_file_acl_high;
			uint16_t		l_uid_high;		/* these 2 fields */
			uint16_t		l_gid_high;		/* were reserved2[0] */
			uint16_t		l_checksum_lo;	/* crc32c(uuid+inum+inode) LE */
			uint16_t		l_reserved;
		} linux2;
		struct {
			uint16_t		h_reserved1;	/* Obsoleted fragment number/size which are removed in ext4 */
			uint16_t		h_mode_high;
			uint16_t		h_uid_high;
			uint16_t		h_gid_high;
			uint32_t		h_author;
		} hurd2;
		struct {
			uint16_t		h_reserved1;	/* Obsoleted fragment number/size which are removed in ext4 */
			uint16_t		m_file_acl_high;
			uint32_t		m_reserved2[2];
		} masix2;
	} osd2;									/* OS dependent 2 */
	uint16_t				extra_isize;
	uint16_t				checksum_hi;	/* crc32c(uuid+inum+inode) BE */
	uint32_t  				ctime_extra;  	/* extra Change time      (nsec << 2 | epoch) */
	uint32_t  				mtime_extra;  	/* extra Modification time(nsec << 2 | epoch) */
	uint32_t  				atime_extra;  	/* extra Access time      (nsec << 2 | epoch) */
	uint32_t  				crtime;       	/* File Creation time */
	uint32_t  				crtime_extra; 	/* extra FileCreationtime (nsec << 2 | epoch) */
	uint32_t  				version_hi;		/* high 32 bits for 64-bit version */
	uint32_t				projid;			/* Project ID */
} Inode;

typedef struct __attribute__((packed)) DirectoryEntry {
	uint32_t inode;
	uint16_t length;
	uint8_t name_len;
	uint8_t file_type;
	char name[];
} DirectoryEntry;

#define DET_FILE_TYPE				0xDE

typedef struct __attribute__((packed)) DirectoryEntryTail {
	uint32_t inode_zero;
	uint16_t lenght;
	uint8_t name_len_zero;
	uint8_t file_type_reserved;
	uint32_t checksum;
} DirectoryEntryTail;

#define EH_MAGIC					0xF30A

typedef struct __attribute__((packed)) ExtentHeader {
	uint16_t magic;
	uint16_t entries;
	uint16_t max;
	uint16_t depth;
	uint32_t generation;
} ExtentHeader;

typedef struct __attribute__((packed)) Extent {
	uint32_t block;
	uint16_t len;
	uint16_t start_hi;
	uint32_t start_lo;
} Extent;

/*DRIVER SPECIFIC STRUCTS*/

typedef struct __attribute__((packed)) ext4_Filesystem {
	volatile PCI_DEV* device;
	uint64_t slba;
	uint64_t block_size;
	uint32_t inodes_per_group;
	uint16_t inodes_size;
	uint16_t sectors_per_block;
	uint64_t sector_size;
	uint32_t seed;
	uint32_t reserved;
} ext4_Filesystem;

typedef struct __attribute__((packed)) InodeCache {
	uint64_t inode_number;
	uint8_t used;
	uint8_t dirty;
	uint16_t mode;
	uint32_t uid;
	uint32_t gid;
	uint16_t link_count;
	uint16_t reserved1;
	uint64_t block_count;
	uint32_t flags;
	uint32_t block_ptr[15];
	uint32_t generation;
	uint8_t reserved2[28]; // align it to 0x80
} InodeCache;

typedef struct __attribute__((packed)) File {
	void* buffer;
	uint64_t size;
} File;

static inline uint64_t block_to_sector(ext4_Filesystem* fs, uint32_t block) {
	return block*fs->sectors_per_block + fs->slba;
}

void mount_filesystem(volatile PCI_DEV* device, PartitionEntry* partition);
#endif