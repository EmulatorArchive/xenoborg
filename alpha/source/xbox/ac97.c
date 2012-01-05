#include "..\\win32.h"
#include "ac97.h"
#include "..\\ds_drv.h"


/* This is primarily just a stub atm.  While the ac97 is fairly straight forward,
   there needs to be more research done before it can be properly emulated */
struct ac97_t			ac97;
struct soundbuffer_t	pcmout[32];

int ac97_init()
{
	int i = 0;

	memset( &ac97, 0, sizeof( struct ac97_t ) );

	/* Clear interrupts */
	ac97.mmio[0x116] = 0xFF;
	ac97.mmio[0x176] = 0xFF;

	/* Create sound buffers for each pcm out descriptor */
	ZeroMemory( pcmout, sizeof( struct soundbuffer_t ) * 32 );
	
	while( i < 32 )
	{
		pcmout[i].wfx.wFormatTag = WAVE_FORMAT_PCM;
		pcmout[i].wfx.nBlockAlign = 4;
		pcmout[i].wfx.nChannels = 2;
		pcmout[i].wfx.nSamplesPerSec = 48000;
		pcmout[i].wfx.nAvgBytesPerSec = 48000 * 4;
		pcmout[i].wfx.wBitsPerSample = 16;
		pcmout[i].dsbdesc.lpwfxFormat = pcmout[i].wfx;
		pcmout[i].dsbdesc.dwBufferBytes = 0;
		i++;
	}

	return 1;
}

uint8_t ac97_read8( uint32_t addr )
{
	return 0;
}

void	ac97_write8( uint32_t addr, uint8_t byte )
{
}

uint16_t ac97_read16( uint32_t addr )
{
	return 0;
}

void	ac97_write16( uint32_t addr, uint16_t word )
{
}

uint32_t ac97_read32( uint32_t addr )
{
	return 0;
}

void	ac97_write32( uint32_t addr, uint32_t dword )
{
}

