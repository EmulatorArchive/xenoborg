#include "..\\win32.h"
#include "..\\gl_drv.h"


uint32_t nv2a_fb_read8( uint32_t addr )
{
	return ((uint8_t*)ogl_get_fb())[addr-0xF0040000];
}

void	 nv2a_fb_write8( uint32_t addr, uint8_t data )
{
	((uint8_t*)ogl_get_fb())[addr-0xF0040000] = data;
}

uint32_t nv2a_fb_read16( uint32_t addr )
{
	return ((uint16_t*)ogl_get_fb())[addr-0xF0040000];
}

void	 nv2a_fb_write16( uint32_t addr, uint16_t data )
{
	((uint16_t*)ogl_get_fb())[addr-0xF0040000] = data;
}

uint32_t nv2a_fb_read32( uint32_t addr )
{
	return ((uint32_t*)ogl_get_fb())[addr-0xF0040000];
}

void	 nv2a_fb_write32( uint32_t addr, uint32_t data )
{
	((uint32_t*)ogl_get_fb())[addr-0xF0040000] = data;
}