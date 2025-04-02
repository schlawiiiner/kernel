#include "../../../src/include/nvme.h"
#include "../../../src/include/mm/memory.h"
#include "../../../src/include/mm/utils.h"
#include "../../../src/include/io.h"
#include "../../../src/include/msi.h"
#include "../../../src/include/apic.h"
#include "../../../src/include/interrupts.h"

void check_completion_status(NVME_CompletionQueueEntry* entry) {
    if ((entry->Status >> 1) != 0x0) {
        print("ERROR: Error Code in completion status");
        printhex(entry->Status >>1);
        //TODO: recover if possible
        while(1);
    }
    return;
}

void ring_submission_queue_tail_doorbell(volatile PCI_DEV* device, int y) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    ControllerProperties* cp = get_controller_properties(device);
    cs->Queues[y].SQT++;
    cp->DBLR[y].SQyTDBL = cs->Queues[y].SQT;
    return;
}

NVME_CompletionQueueEntry* poll_cq(volatile PCI_DEV* device, int y) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    Queue* q = &cs->Queues[y];
    int id = q->CQH;
    int timeout = 0;
    while(1) {
        if (id == 0) {
            if ((q->CQE[id].Status & 0x1) != (q->CQE[id+1].Status & 0x1)) {
                q->CQH++;
                break;
            }
        } else {
            if ((q->CQE[id].Status & 0x1) == (q->CQE[id-1].Status & 0x1)) {
                q->CQH++;
                break;
            }
        }
        if (timeout > 0x10000000) {
            print("ERROR: timeout while polling ACQ");
            while(1);
        }
        timeout++;
    }
    return q->CQE + id;
}

uint16_t pop_cid(NVME_ConfigSpace* cs) {
    if ((cs->CID_stack_ptr >= cs->CID_stack_size) || (cs->CID_stack_ptr >= 0xffff)) {
        //stack is empty
        print("CID stack is empty\n");
        while(1);
    }
    uint16_t cid = cs->CID_stack[cs->CID_stack_ptr];
    cs->CID_stack_ptr++;
    return cid;
}

void push_cid(NVME_ConfigSpace* cs, uint16_t cid) {
    if (cs->CID_stack_ptr <= 0x0) {
        //stack is full
        print("CID stack is full\n");
        while(1);
    }
    cs->CID_stack_ptr--;
    cs->CID_stack[cs->CID_stack_ptr] = cid;
    return;
}

void disable(volatile PCI_DEV* device) {
    ControllerProperties* cp = get_controller_properties(device);
    if (!(cp->CSTS & 0x1)) {
        print("Controller not ready before disabling\n");
        return;
    }
    // Clear the enable bit
    cp->CC &= ~(uint32_t)0x1;
    if (cp->CC & 0x1) {
        print("CC.EN was not cleared\n");
        return;
    }
    // Poll CSTS.RDY to check if the controller disabled successfully
    int timeout = 0;
    while ((cp->CSTS & 0x1)) {
        if (timeout > 0x1000000) {
            print("timeout\n");
            break;
        }
        timeout++;
    }
    return;
}

void enable(volatile PCI_DEV* device) {
    ControllerProperties* cp = get_controller_properties(device);
    cp->CC |= (uint32_t)0x1;
    int timeout = 0;
    while ((cp->CSTS & 0x1) == 0x0) {
        if (timeout > 0x1000000) {
            print("timeout\n");
            break;
        }
        timeout++;
    }
    return;
}

void configure_admin_queues(volatile PCI_DEV* device) {
    ControllerProperties* cp = get_controller_properties(device);
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    uint8_t mpsmin = 0b1111 & (cp->CAP >> 48);
    if (mpsmin != 0x0) {
        //TOTO: handle larger queue sizes. Does malloc return physical continuous memory multiple pages are allocated?
        print("CAP.MPSMIN to high\n");
        return;
    } else {
        cp->CC = cp->CC & ~(uint32_t)(0b1111 << 7);
    }
    uint64_t asq_size = 0x1000;
    uint64_t acq_size = 0x1000;
    uint32_t asqs = 0x1000 / SUBMISSION_QUEUE_ENTRY_SIZE;
    uint32_t acqs = 0x1000 / COMPLETION_QUEUE_ENTRY_SIZE;
    if (asqs >= 4096) {
        print("asqs must be less than 4096");
        return;
    } else if (acqs >= 4096) {
        print("acqs must be less than 4096");
    }
    cp->AQA = asqs | (acqs << 16);

    uint64_t asq = malloc(asq_size);
    memset(asq, 0x0, asq_size);
    uint64_t asq_paddr = get_paddr(asq);
    cs->Queues[0].SQE = (NVME_SubmissionQueueEntry*)asq;
    cs->Queues[0].SQS = (int)asqs;
    cs->Queues[0].SQT = 0;
    cp->ASQ = asq_paddr;

    uint64_t acq = malloc(acq_size);
    memset(acq, 0x0, acq_size);
    uint64_t acq_paddr = get_paddr(acq);
    cs->Queues[0].CQE = (NVME_CompletionQueueEntry*)acq;
    cs->Queues[0].CQS = (int)acqs;
    cs->Queues[0].CQH = 0;
    cp->ACQ = acq_paddr;
}

void check_command_sets(volatile PCI_DEV* device) {
    ControllerProperties* cp = get_controller_properties(device);
    uint8_t bits = ((cp->CAP >> 37) & 0b001) | ((cp->CAP >> 42) & 0b010) | ((cp->CAP >> 43) & 0b100);
    switch (bits) {
    case 0b001:
        cp->CC = (cp->CC & ~(uint32_t)(0b111 << 4)) | (uint32_t)(0b000 << 4);
        break;
    case 0b011:
        cp->CC = (cp->CC & ~(uint32_t)(0b111 << 4)) | (uint32_t)(0b110 << 4);
        break;
    case 0b101:
        cp->CC = (cp->CC & ~(uint32_t)(0b111 << 4)) | (uint32_t)(0b111 << 4);
        break; 
    default:
        print("ERROR: invalid combination of CAP.CSS bits");
        while (1);
        break;
    }
    return;
}


void set_arbitration_mechanism(volatile PCI_DEV* device) {
    ControllerProperties* cp = get_controller_properties(device);
    //Round Robin
    cp->CC = (cp->CC & ~(uint32_t)(0b111 << 11)) | (uint32_t)(0b000 << 11);
}

void init_cid_stack(volatile PCI_DEV* device) {
    NVME_ConfigSpace *cs = get_nvme_config_space(device);
    cs->CID_stack_size = cs->Queues[0].CQS;
    if (cs->CID_stack_size > 0x10000) {
        cs->CID_stack_size = 0x10000;
    }
    cs->CID_stack_ptr = 0;
    cs->CID_stack = (uint16_t*)malloc((uint64_t)(sizeof(uint16_t)*cs->CID_stack_size));
    for (int cid = 0; cid < cs->CID_stack_size; cid++) {
        cs->CID_stack[cid] = (uint16_t)cid;
    }
    return;
}

void set_prp(NVME_SubmissionQueueEntry* cmd, void* buffer, uint32_t size) {
    if (size <= PAGE_SIZE_) {
        cmd->PRP[0] = get_paddr((uint64_t)buffer);
        cmd->PRP[1] = 0x0;
    } else if (size <= 2*PAGE_SIZE_) {
        cmd->PRP[0] = get_paddr((uint64_t)buffer);
        cmd->PRP[1] = get_paddr((uint64_t)buffer + PAGE_SIZE_);
    } else {
        uint64_t n_pages = size/PAGE_SIZE_;
        if (n_pages*PAGE_SIZE_ == size) {
            n_pages--;
        }
        if (n_pages > PAGE_SIZE_/sizeof(uint64_t)) {
            print("ERROR: Current implementation does not support PRP list crossing multiple pages");
            while(1);
        }
        uint64_t* prp_list = (uint64_t*)malloc(0x1000);
        memset((uint64_t)prp_list, 0x0, 0x1000);
        for (uint64_t i = 0; i < n_pages; i++) {
            prp_list[i] = get_paddr((uint64_t)buffer + (1 + i)*PAGE_SIZE_);
        }
        cmd->PRP[0] = get_paddr((uint64_t)buffer);
        cmd->PRP[1] = get_paddr((uint64_t)prp_list);
    }
}


uint16_t identify_command(volatile PCI_DEV* device, void* buffer_vaddr, uint8_t cns, uint32_t nsid, uint16_t cntid, uint16_t csi, uint16_t cnssid, uint8_t uidx) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_SubmissionQueueEntry* cmd = cs->Queues[0].SQE + cs->Queues[0].SQT;
    cmd->Opcode = 0x06;
    cmd->Flags = 0x0;
    cmd->Command_ID = pop_cid(cs);
    cmd->NSID = nsid;
    cmd->CDW[0] = cns | (cntid << 16);
    cmd->CDW[1] = (csi << 24) | cnssid;
    cmd->CDW[4] = 0b1111111 & uidx;
    if ((uint64_t)buffer_vaddr == 0x0) {
        cmd->PRP[0] = 0x0;
        cmd->PRP[1] = 0x0;
    } else {
        cmd->PRP[0] = get_paddr((uint64_t)buffer_vaddr);
        cmd->PRP[1] = 0x0;
    }
    ring_submission_queue_tail_doorbell(device, 0x0);
    return cmd->Command_ID;
}

uint16_t set_features_command(volatile PCI_DEV* device, void* buffer_vaddr, uint8_t fid, uint8_t sv, uint8_t uidx, uint8_t iocsci) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_SubmissionQueueEntry* cmd = cs->Queues[0].SQE + cs->Queues[0].SQT;
    cmd->Opcode = 0x9;
    cmd->Flags = 0x0;
    cmd->Command_ID = pop_cid(cs);
    cmd->CDW[0] = (sv << 31) | 0x0 | fid;
    cmd->CDW[1] = iocsci;
    cmd->CDW[4] = 0b1111111 & uidx;
    if ((uint64_t)buffer_vaddr == 0x0) {
        cmd->PRP[0] = 0x0;
    } else {
        cmd->PRP[0] = get_paddr((uint64_t)buffer_vaddr);
    }
    ring_submission_queue_tail_doorbell(device, 0x0);
    return cmd->Command_ID;
}


void create_iocq_command(volatile PCI_DEV* device, void* buffer, uint16_t qsize, uint16_t qid, uint16_t iv) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_SubmissionQueueEntry* cmd = cs->Queues[0].SQE + cs->Queues[0].SQT;
    cmd->Opcode = 0x5;
    cmd->Flags = 0x0;
    cmd->Command_ID = pop_cid(cs);
    cmd->PRP[0] = get_paddr((uint64_t)buffer);
    cmd->CDW[0] = ((qsize/sizeof(NVME_CompletionQueueEntry) - 1) << 16) | qid; //TODO QUEUE SIZE IN ENTRIES OR IN BYTE, CURRENTLY: ENRIES
    cmd->CDW[1] = (iv << 16) | 0b11;            // PC and IEN set by default
    cmd->CDW[2] = 0x0;
    ring_submission_queue_tail_doorbell(device, 0x0);
}

void create_iosq_command(volatile PCI_DEV* device, void* buffer, uint16_t qsize, uint16_t qid, uint16_t cqid) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_SubmissionQueueEntry* cmd = cs->Queues[0].SQE + cs->Queues[0].SQT;
    cmd->Opcode = 0x1;
    cmd->Flags = 0x0;
    cmd->Command_ID = pop_cid(cs);
    cmd->PRP[0] = get_paddr((uint64_t)buffer);
    cmd->CDW[0] = ((qsize/sizeof(NVME_CompletionQueueEntry) - 1) << 16) | qid; //TODO QUEUE SIZE IN ENTRIES OR IN BYTE, CURRENTLY: ENRIES
    cmd->CDW[1] = (cqid << 16) | 0b01;            // PC set by default
    ring_submission_queue_tail_doorbell(device, 0x0);
}

void identify_namespaces(volatile PCI_DEV* device) {
    uint32_t* namespace_list = (uint32_t*)malloc(0x1000);
    identify_command(device, namespace_list, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0);
    check_completion_status(poll_cq(device, 0));
    int i = 0;
    while(namespace_list[i] != 0x0) {
        IdentifyNamespaceDataStructure* nsds = (IdentifyNamespaceDataStructure*)malloc(0x1000);
        identify_command(device, nsds, 0x0, namespace_list[i], 0x0, 0x0, 0x0, 0x0);
        check_completion_status(poll_cq(device, 0));
        free((uint64_t)nsds, 0x1000);
        i++;
    }
    free((uint64_t)namespace_list, 0x1000);
}


void configure_io_queues(volatile PCI_DEV* device, int number) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_SubmissionQueueEntry* cmd = cs->Queues[0].SQE + cs->Queues[0].SQT;
    cmd->Opcode = 0x9;
    cmd->Flags = 0x0;
    cmd->Command_ID = pop_cid(cs);
    cmd->CDW[0] = 0x7;
    cmd->CDW[1] = ((number-1) << 16) | (number-1);
    cmd->CDW[4] = 0x0;
    ring_submission_queue_tail_doorbell(device, 0x0);
    NVME_CompletionQueueEntry* entry = poll_cq(device, 0);
    check_completion_status(entry);
    uint32_t num = entry->CDW[0];
    uint32_t nsqa = (uint16_t)num + 1;
    uint32_t ncqa = (uint16_t)(num >> 16) + 1; 
    //TODO handle multiple I/O-Queues
    uint32_t max_queue_size = ((cs->CP->CAP & 0xffff) + 1)*sizeof(NVME_CompletionQueueEntry);
    uint64_t buffer = malloc(0x1000);
    //create IOCQ
    memset(buffer, 0x0, 0x1000);
    cs->Queues[1].CQE = (NVME_CompletionQueueEntry*)buffer;
    cs->Queues[1].CQS = (int)(0x1000/sizeof(NVME_CompletionQueueEntry));
    cs->Queues[1].CQH = 0;
    create_iocq_command(device, (void*)buffer, 0x1000, 1, 1);
    check_completion_status(poll_cq(device, 0));

    //create IOSQ
    uint64_t buffer1 = malloc(0x1000);
    memset(buffer1, 0x0, 0x1000);
    cs->Queues[1].SQE = (NVME_SubmissionQueueEntry*)buffer1;
    cs->Queues[1].SQS = (int)(0x1000/sizeof(NVME_SubmissionQueueEntry));
    cs->Queues[1].SQT = 0;

    create_iosq_command(device, (void*)buffer1, 0x1000, 1, 1);
    check_completion_status(poll_cq(device, 0));
}

void set_io_command_set(volatile PCI_DEV* device) {
    ControllerProperties* cp = get_controller_properties(device);
    if ((cp->CAP >> 43) & 0x1) {
        uint64_t buffer = malloc(0x1000);
        identify_command(device, (void*)buffer, 0x1C, 0x0, 0xffff, 0x0, 0x0, 0x0);
        check_completion_status(poll_cq(device, 0));
        uint64_t* command_set_data_struct = (uint64_t*)buffer;
        //TODO: select command set first one is choosen
        set_features_command(device, 0x0 ,0x19, 0x0, 0x0, 0x0);
        check_completion_status(poll_cq(device, 0));
        uint64_t buffer2 = malloc(0x1000);
        identify_command(device, (void*)buffer2, 0x7, 0x0, 0x0, 0x0, 0x0, 0x0);
        check_completion_status(poll_cq(device, 0));
        uint32_t* nsid_list = (uint32_t*)buffer2;
        for (int i = 0; i < 1024; i++) {
            if (nsid_list[i] == 0x0) {
                break;
            }
            uint64_t buffer3 = malloc(0x1000);
            identify_command(device, (void*)buffer3, 0x0, nsid_list[i], 0x0, 0x0, 0x0, 0x0);
            check_completion_status(poll_cq(device, 0));
            IdentifyNamespaceDataStructure* insds = (IdentifyNamespaceDataStructure*)buffer3;
            free(buffer3, 0x1000);
        }
        free(buffer, 0x1000);
        free(buffer2, 0x1000);
    }
    // set queue size entry
    cp->CC = (cp->CC & ~(uint32_t)(0b1111 << 20)) | 0b0100 << 20;
    cp->CC = (cp->CC & ~(uint32_t)(0b1111 << 16)) | 0b0110 << 16;
}

void isr(uint64_t* rsp, uint64_t irq) {
    volatile PCI_DEV* device = get_device_mapped((uint8_t)irq);
    
    send_EOI();
}

void enable_interrupts(volatile PCI_DEV* device) {
    if (!device->msix_cap) {
        print("ERROR: Device does not support MSI-X");
        while(1);
    }
    uint8_t irq = map_isr(isr, device);
    if (!irq) {
        print("ERROR: Failed to request irq");
        while(1);
    }
    enable_MSIX(device, irq);
}

void init_nvme_controller(volatile PCI_DEV* device) {
    if (device->class != 0x010802) {
        return;
    }
    dump_capability(device);
    if (0x0 == device->pcie_cap) {
        print("nvme controller does not support pcie\n");
        return;
    }
    NVME_ConfigSpace* nvme_cs = (NVME_ConfigSpace*)malloc(sizeof(NVME_ConfigSpace));
    device->driver_config_space = nvme_cs;
    nvme_cs->CP = (ControllerProperties*)(device->bars[0].base_address);
    
    // in qemu MSI-X interrupts must be enabled before admin queues are allocated, this is currently a bug???
    enable_interrupts(device);
    // reset controller
    disable(device);
    configure_admin_queues(device);
    check_command_sets(device);
    set_arbitration_mechanism(device);
    enable(device);
    
    init_cid_stack(device);
    IdentifyControllerDataStructure* icds = (IdentifyControllerDataStructure*)malloc(0x1000);
    identify_command(device, icds, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0);
    check_completion_status(poll_cq(device, 0));
    put_chars(icds->MN, 40);
    print("\n");
    set_io_command_set(device);
    identify_namespaces(device);
    configure_io_queues(device, 0x10);
    test_read(device);
}

