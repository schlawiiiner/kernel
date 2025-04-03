#ifndef NVME_H
#define NVME_H
#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"
#include "../../src/include/mp.h"

#define SUBMISSION_QUEUE_ENTRY_SIZE     64
#define COMPLETION_QUEUE_ENTRY_SIZE     16

typedef struct __attribute__((packed)) Doorbell {
    uint32_t SQyTDBL;
    uint32_t CQyHDBL;
} Doorbell;

typedef struct __attribute__((packed)) ControllerProperties {
    volatile uint64_t CAP;
    volatile uint32_t VS;
    volatile uint32_t INTMS;
    volatile uint32_t INTMC;
    volatile uint32_t CC;
    volatile uint32_t Reserved0;
    volatile uint32_t CSTS;
    volatile uint32_t NSSR;
    volatile uint32_t AQA;
    volatile uint64_t ASQ;
    volatile uint64_t ACQ;
    volatile uint32_t CMBLOC;
    volatile uint32_t CMBSZ;
    volatile uint32_t BPINFO;
    volatile uint32_t PBRSEL;
    volatile uint64_t BPMBL;
    volatile uint64_t CMBMSC;
    volatile uint32_t CMBSTS;
    volatile uint32_t CMBEBS;
    volatile uint32_t CMBSWTP;
    volatile uint32_t NSSD;
    volatile uint32_t CRTO;
    volatile uint32_t Reserved1[869];
    volatile uint32_t PMRCAP;
    volatile uint32_t PMRCTL;
    volatile uint32_t PMRSTS;
    volatile uint32_t PMREBS;
    volatile uint32_t PMRSWTP;
    volatile uint32_t PMRMSCL;
    volatile uint32_t PMRMSCU;
    volatile uint32_t Reserved2[121];
    volatile Doorbell DBLR[];
} ControllerProperties;

typedef struct __attribute__((packed)) NVME_SubmissionQueueEntry {
    uint8_t Opcode;
    uint8_t Flags;
    uint16_t Command_ID;
    uint32_t NSID;
    uint32_t CDW2;
    uint32_t CDW3;
    uint64_t Metadata;
    uint64_t PRP[2];
    uint32_t CDW[6];
} NVME_SubmissionQueueEntry;

typedef struct __attribute((packed)) NVME_CompletionQueueEntry {
    uint32_t CDW[2];
    uint16_t SQ_Head_Ptr;
    uint16_t SQ_Identifier;
    uint16_t Command_Identifier;
    volatile uint16_t Status;       // volatile needed for polling phase bit
} NVME_CompletionQueueEntry;

typedef struct __attribute__((packed)) IdentifyControllerDataStructure {
    uint16_t VID;
    uint16_t SSVID;
    char SN[20];
    char MN[40];
    char FR[8];
    uint8_t RAB;
    uint8_t IEEE[3];
    uint8_t CMIC;
    uint8_t MDTS;
    uint16_t CNTLID;
    uint32_t VER;
    uint32_t RTD3R;
    uint32_t RTD3E;
    uint32_t OAES;
    uint32_t CTRATT;
    uint16_t RRLS;
    uint8_t BPCAP;
    uint8_t Reserved;
    uint32_t NSSL;
    uint16_t Reserved2;
    uint8_t PLSI;
    uint8_t CNTRLTYPE;
    uint64_t FGUID[2];
    uint16_t CRDT1;
    uint16_t CRDT2;
    uint16_t CRDT3;
    uint8_t CRCAP;
    uint8_t Reserved3[105];
    uint8_t Reserved4[13];
    uint8_t NVMSR;
    uint8_t VWCI;
    uint8_t MEC;
    uint16_t OACS;
    uint8_t ACL;
    uint8_t AERL;
    uint8_t FRMW;
    uint8_t LPA;
    uint8_t ELPE;
    uint8_t NPSS;
    uint8_t AVSCC;
    uint8_t APSTA;
    uint16_t WCTEMP;
    uint16_t CCTEMP;
    uint16_t MTFA;
    uint32_t HMPRE;
    uint32_t HMMIN;
    uint64_t TNVMCAP[2];
    uint64_t UNVMCAP[2];
    uint32_t RPMBS;
    uint16_t EDSTT;
    uint8_t DSTO;
    uint8_t FWUG;
    uint16_t KAS;
    uint16_t HCTMA;
    uint16_t MNTMT;
    uint16_t MXTMT;
    uint32_t SANICAP;
    uint32_t HMMINDS;
    uint16_t HMMAXD;
    uint16_t NSETIDMAX;
    uint16_t ENDGIDMAX;
    uint8_t ANATT;
    uint8_t ANACAP;
    uint32_t ANAGRPMAX;
    uint32_t NANAGRPIC;
    uint32_t PELS;
    uint16_t DID;
    uint8_t KPIOC;
    uint8_t Reserved5;
    uint16_t MPTFAWR;
    uint8_t Reserved6[6];
    uint64_t MEGCAP[2];
    uint8_t TMPTHHA;
    uint8_t Reserved7;
    uint16_t CQT;
    uint8_t Reserved8[124];
    uint8_t SQES;
    uint8_t CQES;
    uint16_t MAXCMD;
    uint32_t NN;
    uint16_t ONCS;
    uint16_t FUSES;
    uint8_t FNA;
    uint8_t VWC;
    uint16_t AWUN;
    uint16_t AWUPF;
    uint8_t ICSVSCC;
    uint8_t NWPC;
    uint16_t ACWU;
    uint16_t CDFS;
    uint32_t SGLS;
    uint32_t MNAN;
    uint64_t MAXDNA[2];
    uint32_t MAXCNA;
    uint32_t OAQD;
    uint8_t RHIRI;
    uint8_t HIRT;
    uint16_t CMMRTD;
    uint16_t NMMRTD;
    uint8_t MINMRTG;
    uint8_t MAXMRTG;
    uint8_t TRATTR;
    uint8_t Reserved9;
    uint16_t MCUDMQ;
    uint16_t MNSUDMQ;
    uint16_t MCMR;
    uint16_t NMCMR;
    uint16_t MCDQPC;
    uint8_t Reserved10[180];
    uint8_t SUBNQN[256];
    uint8_t Reserved11[768];
    uint32_t IOCCSZ;
    uint32_t IORCSZ;
    uint16_t ICDOFF;
    uint8_t FCATT;
    uint8_t MSDBD;
    uint16_t OFCS;
    uint8_t DCTYPE;
    uint8_t Reserved12[241];
    uint8_t PSD[32][32];
    uint8_t VS[1024];
} IdentifyControllerDataStructure;

typedef struct __attribute__((packed)) IdentifyNamespaceDataStructure {
    uint64_t NSZE;
    uint64_t NCAP;
    uint64_t NUSE;
    uint8_t NSFEAT;
    uint8_t NLBAF;
    uint8_t FLBAS;
    uint8_t MC;
    uint8_t DPC;
    uint8_t DPS;
    uint8_t NMIC;
    uint8_t RESCAP;
    uint8_t FPI;
    uint8_t DLFEAT;
    uint16_t NAWUN;
    uint16_t NAWUMPF;
    uint16_t NACWU;
    uint16_t NABSN;
    uint16_t NABO;
    uint16_t NABSPF;
    uint16_t NOIOB;
    uint64_t NVMCAP[2];
    uint16_t NPWG;
    uint16_t NPWA;
    uint16_t NPDG;
    uint16_t NPDA;
    uint16_t NOWS;
    uint16_t MSSRL;
    uint32_t MCL;
    uint8_t MSRC;
    uint8_t KPIOS;
    uint8_t NULBAF;
    uint8_t Reserved;
    uint32_t KPIODAAG;
    uint32_t Reserved2;
    uint32_t ANAGRPID;
    uint8_t Reserved3[3];
    uint8_t NSATTR;
    uint16_t NVMSETID;
    uint16_t ENDGID;
    uint32_t NGUID;
    uint64_t EUI64;
    uint32_t LBAFn[64];
    uint64_t VS[464];
} IdentifyNamespaceDataStructure;

typedef struct __attribute__((packed)) Queue {
    NVME_SubmissionQueueEntry* SQE;
    NVME_CompletionQueueEntry* CQE;
    int SQS;
    int CQS;
    int SQT;
    int CQH;
} Queue;

/*This is a driver specific struct*/
typedef struct __attribute__((packed)) NVME_ConfigSpace {
    ControllerProperties* CP;
    uint8_t admin_irq;
    uint8_t io_irq;
    uint8_t reserved[2];
    int CID_dequeue;
    int CID_enqueue;
    int CID_queue_size;         /*currently hardcoded to PAGE_SIZE_*/
    uint16_t* CID_queue;       
    uint8_t CID_bitmap[0x100];  /*used for polling, 0x100 = CID_stack_size/sizeof(uint16_t)/sizeof(uint8_t)*/
    Queue Queues[2];
} NVME_ConfigSpace;

typedef struct NVME_IRQ_Mapping NVME_IRQ_Mapping;

/*Completion Queue Status Field*/
#define NVME_STATUS_DNR                 (1 << 15)       /*Do Not Retry*/
#define NVME_STATUS_M                   (1 << 14)       /*More*/
#define NVME_STATUS_CRD                 (0b11 << 12)    /*Command Retry Delay*/
#define NVME_STATUS_CRDT1               (0b01 << 12)    /*Command Retry Delay Time 1*/
#define NVME_STATUS_CRDT2               (0b10 << 12)    /*Command Retry Delay Time 2*/
#define NVME_STATUS_CRDT3               (0b11 << 12)    /*Command Retry Delay Time 3*/
#define NVME_STATUS_SCT                 (0b111 << 9)    /*Status Code Type*/
#define NVME_STATUS_SC                  (0xff << 1)     /*Status Code*/
/*Status Code Type Values*/
#define NVME_SCT_GCS                    (0x0 << 9)      /*Generic Command Status*/
#define NVME_SCT_CSS                    (0x1 << 9)      /*Command Specific Status*/
#define NVME_SCT_MDIE                   (0x2 << 9)      /*Media and Data Integrity Errors*/
#define NVME_SCT_PRS                    (0x3 << 9)      /*Path Related Status*/
#define NVME_SCT_VS                     (0x7 << 9)      /*Vendor Specific*/


static inline ControllerProperties* get_controller_properties(volatile PCI_DEV* device) {
    return ((NVME_ConfigSpace*)(device->driver_config_space))->CP;
}

static inline NVME_ConfigSpace* get_nvme_config_space(volatile PCI_DEV* device) {
    return (NVME_ConfigSpace*)(device->driver_config_space);
}

void init_nvme_controller(volatile PCI_DEV* device);
void test_read(volatile PCI_DEV* device);
void set_prp(NVME_SubmissionQueueEntry* cmd, void* buffer, uint32_t size);
uint16_t pop_cid(NVME_ConfigSpace* cs);
void ring_submission_queue_tail_doorbell(volatile PCI_DEV* device, int y);
void check_completion_status(NVME_CompletionQueueEntry* entry);
void poll_cq(volatile PCI_DEV* device, int cid);
#endif
