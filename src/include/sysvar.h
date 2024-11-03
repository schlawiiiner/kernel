#include "../../src/include/uint.h"
#include "../../src/include/mp.h"


typedef struct __attribute__((packed)) SysTable {
    CPUs* cpus;
} SysTable;

extern volatile SysTable sys_table;