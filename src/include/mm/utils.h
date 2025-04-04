#ifndef MM_UTILS_H
#define MM_UTILS_H
#include "../../../src/include/uint.h"

void memset(uint64_t base_addr, uint64_t value, uint32_t size);
void memcopy(uint64_t* source, uint64_t* destination, int size);
int compare_string(char* str1, char* str2);
#endif