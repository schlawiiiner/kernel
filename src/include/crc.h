#ifndef CRC_H
#define CRC
#include "../../src/include/uint.h"

#define CRC32_SEED              0xffffffff

#define CRC32_POLYNOMIAL        0xedb88320
#define CRC32C_POLYNOMIAL       0x1EDC6F41 

uint32_t calculate_crc32c(void* buffer, uint32_t size, uint32_t seed);
uint32_t calculate_crc32(void* buffer, uint32_t size, uint32_t seed);
#endif