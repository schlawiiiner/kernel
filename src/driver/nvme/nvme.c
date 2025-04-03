#include "../../../src/include/nvme.h"
#include "../../../src/include/mm/memory.h"
#include "../../../src/include/mm/utils.h"
#include "../../../src/include/io.h"
#include "../../../src/include/msi.h"
#include "../../../src/include/apic.h"
#include "../../../src/include/interrupts.h"

void check_completion_status(NVME_CompletionQueueEntry* entry) {
    if ((entry->Status >> 1) != 0x0) {
        print("\nERROR: \tError Code in completion status\n");
        if (entry->Status & NVME_STATUS_DNR) {
            print("\t\tDo Not Retry\n");
        } else {
            switch (entry->Status & NVME_STATUS_CRD) {
            case NVME_STATUS_CRDT1:
                print("\t\tDelay Time 1\n");
                break;
            case NVME_STATUS_CRDT2:
                print("\t\tDelay Time 1\n");
                break;
            case NVME_STATUS_CRDT3:
                print("\t\tDelay Time 1\n");
                break;
            }
        }
        if (entry->Status & NVME_STATUS_M) {
            print("\t\tMore\n");
        }
        switch (entry->Status & NVME_STATUS_SCT) {
        case NVME_SCT_GCS:
            print("\t\tGeneric Command Status\n");
            break;
        case NVME_SCT_CSS:
            print("\t\tCommand Specific Status\n");
            break;
        case NVME_SCT_MDIE:
            print("\t\tMedia and Integrity Errors\n");
            break;
        case NVME_SCT_PRS:
            print("\t\tPath Related Status\n");
            break;
        case NVME_SCT_VS:
            print("\t\tVendor Specific\n");
            break;
        default:
            print("\t\tUnknown Status Type\n");
            break;
        }
        //TODO: try to recover ERROR
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

void ring_completion_queue_head_doorbell(volatile PCI_DEV* device, int y) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    ControllerProperties* cp = get_controller_properties(device);
    cs->Queues[y].CQH++;
    cp->DBLR[y].CQyHDBL = cs->Queues[y].CQH;
    return;
}

/*
'poll_cq' relies on the assumption that it is called right after the command was issued
if the CID already is assigned to another command it results in undefined behaviour, 
although this is very unlikely because the CID are allocated with the policy FIFO
*/
void poll_cq(volatile PCI_DEV* device, int cid) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    int timeout = 0;
    volatile uint8_t* bitmap = &(cs->CID_bitmap[cid >> 3]);
    while(1) {
        if (!(*bitmap & (1 << (cid & 0b111)))) {
            return;
        }
        timeout++;
        if (timeout > 0x1000000) {
            print("ERROR: timeout while polling CID: ");
            printhex(cid);
            while(1);
        }
    }
}

NVME_CompletionQueueEntry* poll_cq_entry(volatile PCI_DEV* device, int cid, int y) {
    poll_cq(device, cid);
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    int current_stack_idx = cs->Queues[y].CQH;
    for (int i = current_stack_idx; i >= 0; i--) {
        if (cs->Queues[y].CQE[i].Command_Identifier == cid) {
            return &(cs->Queues[y].CQE[i]);
        }
    }
    print("ERROR: Could not find Completion Queue Entry corresponding to CID: ");
    printhex(cid);
    while(1);
}

uint16_t pop_cid(NVME_ConfigSpace* cs) {
    if (cs->CID_dequeue == cs->CID_enqueue) {
        //stack is empty
        print("CID queue is empty\n");
        while(1);
    }
    uint16_t cid = cs->CID_queue[cs->CID_dequeue];
    cs->CID_dequeue++;
    if (cs->CID_dequeue == cs->CID_queue_size) {
        //wrap around
        cs->CID_dequeue = 0x0;
    }
    cs->CID_bitmap[cid >> 3] |= (uint8_t)(1 << (cid & 0b111));
    return cid;
}

void push_cid(NVME_ConfigSpace* cs, uint16_t cid) {
    cs->CID_enqueue++;
    if (cs->CID_enqueue == cs->CID_queue_size) {
        //wrap around
        cs->CID_enqueue = 0x0;
    }
    if (cs->CID_enqueue == cs->CID_dequeue) {
        //queue is full
        print("CID queue is full\n");
        while(1);
    }
    cs->CID_queue[cs->CID_enqueue] = cid;
    cs->CID_bitmap[cid >> 3] &= ~(uint8_t)(1 << (cid & 0b111));
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
    cs->CID_queue_size = PAGE_SIZE_/sizeof(uint16_t);
    cs->CID_dequeue = 0;
    cs->CID_enqueue = cs->CID_queue_size - 1;
    cs->CID_queue = (uint16_t*)malloc((uint64_t)(sizeof(uint16_t)*cs->CID_queue_size));
    for (int cid = 0; cid < cs->CID_queue_size; cid++) {
        cs->CID_queue[cid] = (uint16_t)cid;
    }
    for (int i = 0; i < sizeof(cs->CID_bitmap); i++) {
        cs->CID_bitmap[i] = 0x0;
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


uint16_t create_iocq_command(volatile PCI_DEV* device, void* buffer, uint16_t qsize, uint16_t qid, uint16_t iv) {
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
    return cmd->Command_ID;
}

uint16_t create_iosq_command(volatile PCI_DEV* device, void* buffer, uint16_t qsize, uint16_t qid, uint16_t cqid) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_SubmissionQueueEntry* cmd = cs->Queues[0].SQE + cs->Queues[0].SQT;
    cmd->Opcode = 0x1;
    cmd->Flags = 0x0;
    cmd->Command_ID = pop_cid(cs);
    cmd->PRP[0] = get_paddr((uint64_t)buffer);
    cmd->CDW[0] = ((qsize/sizeof(NVME_CompletionQueueEntry) - 1) << 16) | qid; //TODO QUEUE SIZE IN ENTRIES OR IN BYTE, CURRENTLY: ENRIES
    cmd->CDW[1] = (cqid << 16) | 0b01;            // PC set by default
    ring_submission_queue_tail_doorbell(device, 0x0);
    return cmd->Command_ID;
}

void identify_namespaces(volatile PCI_DEV* device) {
    uint32_t* namespace_list = (uint32_t*)malloc(0x1000);
    uint16_t cid = identify_command(device, namespace_list, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0);
    poll_cq(device, cid);
    int i = 0;
    while(namespace_list[i] != 0x0) {
        IdentifyNamespaceDataStructure* nsds = (IdentifyNamespaceDataStructure*)malloc(0x1000);
        cid = identify_command(device, nsds, 0x0, namespace_list[i], 0x0, 0x0, 0x0, 0x0);
        poll_cq(device, cid);
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
    NVME_CompletionQueueEntry* entry = poll_cq_entry(device, cmd->Command_ID, 0x0);
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
    uint16_t cid = create_iocq_command(device, (void*)buffer, 0x1000, 1, 1);
    poll_cq(device, cid);

    //create IOSQ
    uint64_t buffer1 = malloc(0x1000);
    memset(buffer1, 0x0, 0x1000);
    cs->Queues[1].SQE = (NVME_SubmissionQueueEntry*)buffer1;
    cs->Queues[1].SQS = (int)(0x1000/sizeof(NVME_SubmissionQueueEntry));
    cs->Queues[1].SQT = 0;

    cid = create_iosq_command(device, (void*)buffer1, 0x1000, 1, 1);
    poll_cq(device, cid);
}

void set_io_command_set(volatile PCI_DEV* device) {
    ControllerProperties* cp = get_controller_properties(device);
    if ((cp->CAP >> 43) & 0x1) {
        uint64_t buffer = malloc(0x1000);
        uint16_t cid = identify_command(device, (void*)buffer, 0x1C, 0x0, 0xffff, 0x0, 0x0, 0x0);
        poll_cq(device, cid);
        uint64_t* command_set_data_struct = (uint64_t*)buffer;
        //TODO: select command set first one is choosen
        cid = set_features_command(device, 0x0 ,0x19, 0x0, 0x0, 0x0);
        poll_cq(device, cid);
        uint64_t buffer2 = malloc(0x1000);
        cid = identify_command(device, (void*)buffer2, 0x7, 0x0, 0x0, 0x0, 0x0, 0x0);
        poll_cq(device, cid);
        uint32_t* nsid_list = (uint32_t*)buffer2;
        for (int i = 0; i < 1024; i++) {
            if (nsid_list[i] == 0x0) {
                break;
            }
            uint64_t buffer3 = malloc(0x1000);
            cid = identify_command(device, (void*)buffer3, 0x0, nsid_list[i], 0x0, 0x0, 0x0, 0x0);
            poll_cq(device, cid);
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


void admin_isr(uint64_t* rsp, uint64_t irq) {
    volatile PCI_DEV* device = get_device_mapped((uint8_t)irq);
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_CompletionQueueEntry* entry = &(cs->Queues[0].CQE[cs->Queues[0].CQH]);
    check_completion_status(entry);
    ring_completion_queue_head_doorbell(device, 0x0);
    push_cid(cs, entry->Command_Identifier);
    send_EOI();
}
void io_isr(uint64_t* rsp, uint64_t irq) {
    volatile PCI_DEV* device = get_device_mapped((uint8_t)irq);
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_CompletionQueueEntry* entry = &(cs->Queues[1].CQE[cs->Queues[1].CQH]);
    check_completion_status(entry);
    ring_completion_queue_head_doorbell(device, 0x1 + get_apic_id());
    push_cid(cs, entry->Command_Identifier);
    send_EOI();
}


void enable_interrupts(volatile PCI_DEV* device) {
    if (msix_support(device)) {
        print("ERROR: NVMe Controller does not Support MSI-X\n");
        while(1);
    }
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    cs->admin_irq = map_isr(admin_isr, device);
    cs->io_irq = map_isr(io_isr, device);

    if (!cs->admin_irq || !cs->io_irq) {
        print("ERROR: Failed to request irq");
        while(1);
    }
    msix_setup_table(device);
    msix_configure_vector(device, 0x0, 0x0, cs->admin_irq);
    msix_configure_vector(device, 0x1, 0x0, cs->io_irq);
    msix_enable(device);
    msix_unmask_vector(device, 0x0);
    msix_unmask_vector(device, 0x1);
    msix_unmask_function(device);
    msix_enable(device);
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
    
    // in qemu MSI-X interrupts must be enabled before admin queues are allocated, this is currently a qemu bug???
    enable_interrupts(device);
    // reset controller
    disable(device);
    configure_admin_queues(device);
    check_command_sets(device);
    set_arbitration_mechanism(device);
    enable(device);
    
    init_cid_stack(device);
    IdentifyControllerDataStructure* icds = (IdentifyControllerDataStructure*)malloc(0x1000);
    uint16_t cid = identify_command(device, icds, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0);
    poll_cq(device, cid);
    put_chars(icds->MN, 40);
    print("\n");
    set_io_command_set(device);
    identify_namespaces(device);
    configure_io_queues(device, 0x10);
    test_read(device);
}

