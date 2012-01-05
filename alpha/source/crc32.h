#pragma once

#include <stdint.h>

#define POLYNOMIAL		0x04C11DB7
#define CRC_BUFFER_SIZE	4096

#ifdef __cplusplus
extern "C" {
#endif

/* 32-bit CRC code functions */
void crc_generate_tables();
uint32_t crc_update( uint32_t crc_accum, uint8_t* ptr, int size );

#ifdef __cplusplus
}
#endif
