#include <stdio.h>
#include <stdint.h>
#include "crc32.h"

/* CRC table */
uint32_t crc_table[256];


void crc_generate_tables()
{
	uint16_t i, j;
	uint32_t crc_accum;

	for( i = 0; i < 256; i++ )
	{
		crc_accum = ( (uint32_t) i << 24 );
		for( j = 0; j < 8; j++ )
		{
			if( crc_accum & 0x80000000 )
				crc_accum = ( crc_accum << 1 ) ^ POLYNOMIAL;
			else
				crc_accum = ( crc_accum << 1 );
		}
		crc_table[i] = crc_accum;
	}
}

uint32_t crc_update( uint32_t crc_accum, uint8_t* ptr, int size )
{
	uint32_t i;
	int j;

	for( j = 0; j < size; j++ )
	{
		i = ((int) ( crc_accum >> 24 ) ^ *ptr++ ) & 0xFF;
		crc_accum = ( crc_accum << 8 ) ^ crc_table[i];
	}
	crc_accum = ~crc_accum;

	return crc_accum;
}
