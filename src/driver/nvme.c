#include "../../src/include/nvme.h"
#include "../../src/include/mm/memory.h"
#include "../../src/include/mm/utils.h"
#include "../../src/include/io.h"
#include "../../src/include/msi.h"
#include "../../src/include/apic.h"
#include "../../src/include/interrupts.h"

inline ControllerProperties* get_controller_properties(volatile PCI_DEV* device) {
    return ((NVME_ConfigSpace*)(device->driver_config_space))->CP;
}

inline NVME_ConfigSpace* get_nvme_config_space(volatile PCI_DEV* device) {
    return (NVME_ConfigSpace*)(device->driver_config_space);
}

void ring_admin_submission_queue_doorbell(volatile PCI_DEV* device) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    cs->ASQ_tail++;
    uint32_t* SQ0TDBL = (uint32_t*)((uint64_t)get_controller_properties(device) + 0x1000);
    SQ0TDBL[0] = cs->ASQ_tail;
    return;
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
    cs->ASQ_vaddr = (NVME_SubmissionQueueEntry*)asq;
    cs->ASQ_size = (int)asqs;
    cs->ASQ_tail = 0;
    cp->ASQ = asq_paddr;

    uint64_t acq = malloc(acq_size);
    memset(acq, 0x0, acq_size);
    uint64_t acq_paddr = get_paddr(acq);
    cs->ACQ_vaddr = (NVME_CompletionQueueEntry*)acq;
    cs->ACQ_size = (int)acqs;
    cs->ACQ_head = 0;
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
    cs->CID_stack_size = cs->ACQ_size;
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

uint16_t identify_command(volatile PCI_DEV* device, void* buffer_vaddr, uint8_t cns, uint32_t nsid, uint16_t cntid, uint16_t csi, uint16_t cnssid, uint8_t uidx) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_SubmissionQueueEntry* cmd = cs->ASQ_vaddr + cs->ASQ_tail;
    cmd->Opcode = 0x06;
    cmd->Flags = 0x0;
    cmd->Command_ID = pop_cid(cs);
    cmd->NSID = nsid;
    cmd->CDW[0] = cns | (cntid << 16);
    cmd->CDW[1] = (csi << 24) | cnssid;
    cmd->CDW[4] = 0b1111111 & uidx;
    if ((uint64_t)buffer_vaddr == 0x0) {
        cmd->PRP[0] = 0x0;
    } else {
        cmd->PRP[0] = get_paddr((uint64_t)buffer_vaddr);
    }
    ring_admin_submission_queue_doorbell(device);
    return cmd->Command_ID;
}

uint16_t set_features_command(volatile PCI_DEV* device, void* buffer_vaddr, uint8_t fid, uint8_t sv, uint8_t uidx, uint8_t iocsci) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_SubmissionQueueEntry* cmd = cs->ASQ_vaddr + cs->ASQ_tail;
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
    ring_admin_submission_queue_doorbell(device);
    return cmd->Command_ID;
}
void identify_namespaces(volatile PCI_DEV* device) {
    uint32_t* namespace_list = (uint32_t*)malloc(0x1000);
    identify_command(device, namespace_list, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0);
    for (int i = 0; i < 0x40000000; i++) {
        asm volatile("nop");
    }
    int i = 0;
    while(namespace_list[i] != 0x0) {
        IdentifyNamespaceDataStructure* nsds = (IdentifyNamespaceDataStructure*)malloc(0x1000);
        identify_command(device, nsds, 0x0, namespace_list[i], 0x0, 0x0, 0x0, 0x0);
        for (int i = 0; i < 0x40000000; i++) {
            asm volatile("nop");
        }
        printhex(nsds->NSZE);
        i++;
    }
}

void configure_io_queues(volatile PCI_DEV* device, int number) {
    NVME_ConfigSpace* cs = get_nvme_config_space(device);
    NVME_SubmissionQueueEntry* cmd = cs->ASQ_vaddr + cs->ASQ_tail;
    cmd->Opcode = 0x9;
    cmd->Flags = 0x0;
    cmd->Command_ID = pop_cid(cs);
    cmd->CDW[0] = 0x7;
    cmd->CDW[1] = ((number-1) << 16) | (number-1);
    cmd->CDW[4] = 0x0;
    ring_admin_submission_queue_doorbell(device);
    uint16_t cid = cmd->Command_ID;
    for (int i = 0; i < 0x40000000; i++) {
        asm volatile("nop");
    }
    int i = 0;
    while (get_nvme_config_space(device)->ACQ_vaddr[i].Command_Identifier != cid) {
        i++;
    }
    uint32_t num = get_nvme_config_space(device)->ACQ_vaddr[i].CDW[0];
    uint32_t nsqa = (uint16_t)num + 1;
    uint32_t ncqa = (uint16_t)(num >> 16) + 1;
    
}

void test(uint64_t* rsp, uint64_t irq) {
    
    print("recieved Interrupt\n");
    send_EOI();
}

void init_nvme_controller(volatile PCI_DEV* device) {
    if (device->class != 0x010802) {
        return;
    }
    dump_capability(device);
    if (0x0 == device->pcie_cap_offset) {
        print("nvme controller does not support pcie\n");
        return;
    }
    NVME_ConfigSpace* nvme_cs = (NVME_ConfigSpace*)malloc(sizeof(NVME_ConfigSpace));
    device->driver_config_space = nvme_cs;
    nvme_cs->CP = (ControllerProperties*)(device->bars[0].base_address);
    
    /* 
    in qemu MSI-X interrupts must be enabled before admin queues are allocated, this is currently a bug???
    */
    map_isr(0x23, test);
    enable_MSIX(device);

    disable(device);
    configure_admin_queues(device);
    check_command_sets(device);
    set_arbitration_mechanism(device);
    enable(device);
    
    init_cid_stack(device);
    IdentifyControllerDataStructure* icds = (IdentifyControllerDataStructure*)malloc(0x1000);
    identify_command(device, icds, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0);
    for (int i = 0; i < 0x40000000; i++) {
        asm volatile("nop");
    }
    for (int i = 0; i < 40; i++) {
        put_char(icds->MN[i]);
    }
    print("\n");
    //identify_namespaces(device);
    configure_io_queues(device, 1000);
}

