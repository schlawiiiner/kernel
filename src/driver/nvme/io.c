#include "../../../src/include/nvme.h"
#include "../../../src/include/mm/memory.h"

#define LIMITED_RETRY           (1 << 31)
#define FORCE_UNIT_ACCESS       (1 << 30)
#define STORAGE_TAG_CHECK       (1 << 24)

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
    cmd->PRP[0] = get_paddr(buffer);
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

void test_read(volatile PCI_DEV* device) {
    uint64_t buffer = malloc(0x4000);
    uint64_t metadata = malloc(0x1000);
    read_command(device, 1, buffer, metadata, 0x0, 0x3, 0x1);
    uint64_t* bf = (uint64_t*)buffer;
    for (int i = 0; i < 512*4/8; i+=8) {
        printhex(bf[i]);
        print(" ");
        printhex(bf[i+1]);
        print(" ");
        printhex(bf[i+2]);
        print(" ");
        printhex(bf[i+3]);
        print(" ");
        printhex(bf[i+4]);
        print(" ");
        printhex(bf[i+5]);
        print(" ");
        printhex(bf[i+6]);
        print(" ");
        printhex(bf[i+7]);
        print("\n");
    }
    return;
}