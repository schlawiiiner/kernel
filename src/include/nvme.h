#ifndef NVME_H
#define NVME_H
#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"

#define SUBMISSION_QUEUE_ENTRY_SIZE     64
#define COMPLETION_QUEUE_ENTRY_SIZE     16

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
} ControllerProperties;

typedef struct __attribute__((packed)) NVME_SubmissionQueueEntry {
    uint8_t Opcode;
    uint8_t Flags;
    uint16_t Command_ID;
    uint32_t NSID;
    uint64_t Reserved;
    uint64_t Metadata;
    uint64_t PRP[2];
    uint32_t CDW[6];
} NVME_SubmissionQueueEntry;

typedef struct __attribute((packed)) NVME_CompletionQueueEntry {
    uint32_t CDW[2];
    uint16_t SQ_Head_Ptr;
    uint16_t SQ_Identifier;
    uint16_t Command_Identifier;
    uint16_t Status;
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

typedef struct __attribute__((packed)) NVME_ConfigSpace {
    ControllerProperties* CP;
    NVME_SubmissionQueueEntry* ASQ_vaddr;
    NVME_CompletionQueueEntry* ACQ_vaddr;
    int ASQ_size;
    int ACQ_size;
    int ASQ_tail;
    int ACQ_head;
    uint16_t* CID_stack;
    int CID_stack_ptr;
    int CID_stack_size;
} NVME_ConfigSpace;

void init_nvme_controller(volatile PCI_DEV* device);
#endif
