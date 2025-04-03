#include "../../../src/include/nvme.h"
#include "../../../src/include/mm/memory.h"
#include "../../../src/include/gpt.h"
#include "../../../src/include/ext4.h"
#include "../../../src/include/crc.h"
#include "../../../src/include/io.h"

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