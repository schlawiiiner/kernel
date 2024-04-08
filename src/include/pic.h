#ifndef PIC
#define PIC
#include "../../src/include/uint.h"
#include "../../src/include/io.h"

void PIC_sendEOI(uint8_t irq);
void PIC_remap(int offset1, int offset2);

#endif