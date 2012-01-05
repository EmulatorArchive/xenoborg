#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* NV2A emulation functions */
uint8_t nv2a_read8( uint32_t addr );
void	nv2a_write8( uint32_t addr, uint8_t data );
uint16_t nv2a_read16( uint32_t addr );
void	nv2a_write16( uint32_t addr, uint16_t data );
uint32_t nv2a_read32( uint32_t addr );
void	nv2a_write32( uint32_t addr, uint32_t data );

#ifdef __cplusplus
}
#endif
