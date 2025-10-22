#ifndef UINT32_ARRAYS
#define UINT32_ARRAYS

#include"libs.h"

uint32_t** alloc_uint32_array(uint16_t size, char* error_mesage_array_name);

void free_uint32_array(uint32_t** array);

uint8_t compare_uint32_arrays(uint32_t** array_A, uint32_t** array_B, uint16_t size);

#endif
