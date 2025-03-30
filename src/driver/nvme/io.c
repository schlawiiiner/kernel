#include "../../../src/include/nvme.h"
#include "../../../src/include/mm/memory.h"
#include "../../../src/include/gpt.h"
#include "../../../src/include/io.h"

#define LIMITED_RETRY           (1 << 31)
#define FORCE_UNIT_ACCESS       (1 << 30)
#define STORAGE_TAG_CHECK       (1 << 24)

#define GPT_CRC_POLY            0x04c11db7
#define BLOCK_SIZE              0x200

static uint32_t crc32_table[256];
static int crc32_initialized = 0;

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
    set_prp(cmd, (void*)buffer, nlb*BLOCK_SIZE);
    cmd->CDW2 = 0x0;
    cmd->CDW3 = 0x0;
    slba = get_paddr(slba);
    cmd->CDW[0] = (uint32_t)slba;
    cmd->CDW[1] = (uint32_t)(slba >> 32);
    cmd->CDW[2] = nlb;
    cmd->CDW[3] = 0x0;
    cmd->CDW[4] = 0x0;
    cmd->CDW[5] = 0x0;

    ring_submission_queue_tail_doorbell(device, y);
    return cmd->Command_ID;
}

void crc32_init(void) {
    if (crc32_initialized) return;
	uint32_t crc32 = 1;
    for (unsigned int i = 128; i; i >>= 1) {
		crc32 = (crc32 >> 1) ^ (crc32 & 1 ? 0xedb88320 : 0);
		for (unsigned int j = 0; j < 256; j += 2*i)
        	crc32_table[i + j] = crc32 ^ crc32_table[j];
	}
    crc32_initialized = 1;
}

uint32_t calculate_checksum(PartitionTableHeader* header) {
    crc32_init(); // Ensure table is initialized

    uint32_t checksum_copy = header->Checksum;
    header->Checksum = 0; // Zero out checksum field

    const uint8_t* data = (const uint8_t*)header;
    uint32_t crc = 0xFFFFFFFF;

    for (int i = 0; i < 0x5C; i++) {
        crc ^= data[i];
        crc = (crc >> 8) ^ crc32_table[crc & 0xff];
    }

    header->Checksum = checksum_copy; // Restore original checksum
    return crc ^ 0xFFFFFFFF;
}

void read_partition_table(volatile PCI_DEV* device, PartitionTableHeader* header) {
    int n = header->Partition_Number;
    uint64_t size = n*header->Partition_Entry_Size;
    uint16_t nlb = size/512;
    uint64_t buffer = malloc(size);
    memset(buffer, 0x0, size);
    read_command(device, 1, buffer, 0x1, header->Starting_LBA_Partition_Table, nlb, 0x1); //TODO: if nlb instead of 0x1 then ERROR Status
    check_completion_status(poll_cq(device, 1));

    PartitionEntry* partition_table = (PartitionEntry*)buffer;
    for (int i = 0; i < n; i++) {
        if ((partition_table[i].Partition_Type_GUID[0] | partition_table[i].Partition_Type_GUID[1]) == 0x0) {
            continue;
        } else {
            printhex(partition_table[i].Starting_LBA);
            print("\n");
        }
    }
    free(buffer, size);
}

void test_read(volatile PCI_DEV* device) {
    uint64_t buffer = malloc(0x4000);
    uint64_t metadata = malloc(0x1000);
    read_command(device, 1, buffer, metadata, 0x1, 0x0, 0x1);
    check_completion_status(poll_cq(device, 1));
    PartitionTableHeader* header = (PartitionTableHeader*)buffer;
    if (header->Signature != GPT_SIGNATURE) {
        print("ERROR: GPT Signature wrong");
        while(1);
    }
    if (header->Checksum != calculate_checksum(header)) {
        print("ERROR: Invalid checksum");
        while(1);
    }
    read_partition_table(device, header);
    return;
}