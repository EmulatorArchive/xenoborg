#pragma once

/* Framebuffer read/write functions */
uint8_t nv2a_fb_read8( uint32_t addr );
void	 nv2a_fb_write8( uint32_t addr, uint8_t data );
uint16_t nv2a_fb_read16( uint32_t addr );
void	 nv2a_fb_write16( uint32_t addr, uint16_t data );
uint32_t nv2a_fb_read32( uint32_t addr );
void	 nv2a_fb_write32( uint32_t addr, uint32_t data );

/* Framebuffer addresses */
extern const uint32_t fb_start;
extern const uint32_t fb_start2;