#ifndef NVME_H
#define NVME_H
#include "../../src/include/uint.h"
#include "../../src/include/graphics.h"
#include "../../src/include/pci.h"

typedef struct __attribute__((packed)) ControllerProperties {
    uint64_t CAP;
    uint32_t VS;
    uint32_t INTMS;
    uint32_t INTMC;
    uint32_t CC;
    uint32_t Reserved0;
    uint32_t CSTS;
    uint32_t NSSR;
    uint32_t AQA;
    uint64_t ASQ;
    uint64_t ACQ;
    uint32_t CMBLOC;
    uint32_t CMBSZ;
    uint32_t BPINFO;
    uint32_t PBRSEL;
    uint64_t BPMBL;
    uint64_t CMBMSC;
    uint32_t CMBSTS;
    uint32_t CMBEBS;
    uint32_t CMBSWTP;
    uint32_t NSSD;
    uint32_t CRTO;
    uint32_t Reserved1[869];
    uint32_t PMRCAP;
    uint32_t PMRCTL;
    uint32_t PMRSTS;
    uint32_t PMREBS;
    uint32_t PMRSWTP;
    uint32_t PMRMSCL;
    uint32_t PMRMSCU;
    uint32_t Reserved2[121];
} ControllerProperties;


void init_nvme_controller(volatile PCI_DEV* nvme_controller);
#endif
