#include "win32.h"
#include "ds_drv.h"


/* DirectSound interfaces */
IDirectSound8*			ds = NULL;


int		enable_dsdrv( struct window_t* wnd )
{
	/* Initialize DirectSound 8.0 */
	HRESULT hr = DirectSoundCreate8( NULL, &ds, NULL );
	if( FAILED( hr ) )
		return 0;

	/* set cooperative level */
	hr = IDirectSound8_SetCooperativeLevel( ds, wnd->hwnd, DSSCL_NORMAL );

	return 1;
}

void	disable_dsdrv()
{
	/* uninitialize DirectSound */
	if(ds)
		IDirectSound8_Release(ds);
}

int ds_createbuffer( struct soundbuffer_t* buffer )
{
	/* Create a DirectSound buffer */
	HRESULT hr = IDirectSound8_CreateSoundBuffer( ds, &buffer->dsbdesc, &buffer->dsb, NULL );
	if( FAILED( hr ) )
		return 0;

	return 1;
}