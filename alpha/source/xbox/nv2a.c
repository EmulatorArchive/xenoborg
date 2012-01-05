#include "..\\win32.h"
#include "nv2a.h"
#include "nv2afb.h"

const uint32_t fb_start = 0xF0040240;
const uint32_t fb_start2 = 0x8003d000;

uint8_t nv2a_read8( uint32_t addr )
{
//	printf( "nv2a_read8\n" );
	if( addr >= fb_start && addr < (fb_start+(640*480*4)) )
		return nv2a_fb_read8( addr );
	if( addr >= fb_start2 && addr < (fb_start2+(640*480*4)) )
		return nv2a_fb_read8( addr );

	return 0;
}

void	nv2a_write8( uint32_t addr, uint8_t data )
{
//	printf( "nv2a_write8\n" );
	if( addr >= fb_start && addr < (fb_start+(640*480*4)) )
		nv2a_fb_write8( addr, data );
	if( addr >= fb_start2 && addr < (fb_start2+(640*480*4)) )
		nv2a_fb_write8( addr, data );
}

uint16_t nv2a_read16( uint32_t addr )
{
	return 0;
}

void	nv2a_write16( uint32_t addr, uint16_t data )
{
}

uint32_t nv2a_read32( uint32_t addr )
{
	printf( "nv2a_read32\n" );
	if( addr >= fb_start && addr < (fb_start+(640*480*4)) )
		return nv2a_fb_read32( addr );
	if( addr >= fb_start2 && addr < (fb_start2+(640*480*4)) )
		return nv2a_fb_read32( addr );

	return 0;
}

void	nv2a_write32( uint32_t addr, uint32_t data )
{
	printf( "nv2a_write32\n" );
	if( addr >= fb_start && addr < (fb_start+(640*480*4)) )
		nv2a_fb_write32( addr, data );
	if( addr >= fb_start2 && addr < (fb_start2+(640*480*4)) )
		nv2a_fb_write32( addr, data );
}
