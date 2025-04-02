#include "../../src/include/crc.h"


static uint32_t crc32_table[256];
static uint32_t crc32_initialized = 0;

static uint32_t crc32c_table[256];
static uint32_t crc32c_initialized = 0;


void crc32_init(void) {
    if (crc32_initialized) return;
	uint32_t crc32 = 1;
    for (unsigned int i = 128; i; i >>= 1) {
		crc32 = (crc32 >> 1) ^ (crc32 & 1 ? CRC32_POLYNOMIAL : 0);
		for (unsigned int j = 0; j < 256; j += 2*i)
        	crc32_table[i + j] = crc32 ^ crc32_table[j];
	}
    crc32_initialized = 1;
}

uint32_t reflect(uint32_t value, uint32_t bits) {
    uint32_t result = 0;
    for (uint32_t i = 0; i < bits; i++) {
        result = (result << 1) | (value & 1);
        value >>= 1;
    }
    return result;
}

void crc32c_init(void) {
    if (crc32c_initialized) return;
	uint32_t crc32c = 0;
    for (int i = 0; i < 256; i++) {
        crc32c = reflect(i, 8) << 24;
        for (int j = 0; j < 8; j++) {
            crc32c = (crc32c << 1) ^ (crc32c & 0x80000000 ? CRC32C_POLYNOMIAL : 0);
        }
        crc32c_table[i] = reflect(crc32c & 0xffffffff, 32);
    }
    crc32c_initialized = 1;
}

uint32_t calculate_crc32(void* buffer, uint32_t size, uint32_t seed) {
    crc32_init();
    const uint8_t *data = buffer;
    uint32_t crc = seed ^ 0xffffffff;
    for (uint32_t i = 0; i < size; i++) {
        crc ^= data[i];
        crc = (crc >> 8) ^ crc32_table[crc & 0xff];
    }
    return crc ^ 0xffffffff;
}

uint32_t calculate_crc32c(void* buffer, uint32_t size, uint32_t seed) {
    crc32c_init();
    const uint8_t *data = buffer;
    uint32_t crc = seed ^ 0xffffffff;
    for (uint32_t i = 0; i < size; i++) {
        crc = (crc >> 8) ^ crc32c_table[(crc ^ data[i]) & 0xff];
    }
    return crc ^ 0xffffffff;
}