#pragma once

/* DirectSound include(s) */
#include <dsound.h>

/* Sound buffer structure */
struct soundbuffer_t
{
	IDirectSoundBuffer* dsb;
	WAVEFORMATEX wfx;
	DSBUFFERDESC dsbdesc;
};

/* DirectSound functions */
int		enable_dsdrv( struct window_t* wnd );
void	disable_dsdrv();
